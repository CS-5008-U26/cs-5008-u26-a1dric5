/* This file is the program entry point: 
file parser and built-in simulator

Two ways to run the program:

./orderbook events.txt
Reads order events from a plain-text file, one per line:
    LIMIT BUY <price> <qty>
	LIMIT SELL <price> <qty>
    MARKET BUY <qty>
	MARKET SELL <qty>
    CANCEL BUY <price> <id>     (side and price locate the level;
	CANCEL SELL <price> <id>     (the id picks the order within it)
	BOOK [depth]                (print the top of the book)
Blank lines and lines starting with # are ignored.

./orderbook --sim <numEvents> <trend> [seed]
Generates a random-walk order flow. 
<trend> is a percentage from -100 to 100: 0 drifts randomly, 
100 trends straight up, -100 straight down.  
A strong trend feeds the trees near-sorted prices, which is exactly 
the input that ruins a plain BST, and the run report shows the AVL 
tree shrugging it off. */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "match.h"

#define LINE_MAX_LEN 128    // longest input line we accept

/* Shared reporting

treeCount		        counts the nodes in a subtree
Parameters:
struct PriceLevel *r	root of the subtree

Returns:
long	                number of nodes

Notes:
A plain recursive traversal: a tree's size is 1 (for the root)
plus the sizes of its two subtrees. */
long treeCount(struct PriceLevel *r) {
    if (r == NULL) {
        return (0);
    }
    return (1 + treeCount(r->left) + treeCount(r->right));
}

/* reportTree		        prints size/height stats for one side of the book
Parameters:
char *name			        label to print ("BID" or "ASK")
struct PriceLevel *root		root of that side's tree

Returns:
nothing (void)

Notes:
Also runs the full invariant checker, so every run of the program
doubles as a correctness test of the AVL code. */
void reportTree(char *name, struct PriceLevel *root) {
    long count = treeCount(root);
    int ok = levelCheckInvariants(root, LONG_MIN, LONG_MAX);
    printf("%s tree: %ld price levels, height %d, invariants %s\n",
           name, count, levelHeight(root), (ok >= 0) ? "OK" : "VIOLATED");
}

// Mode 1: read events from a file

#define MAX_TOKENS 4            // longest command is 4 words: CANCEL BUY 4997 6
#define TOKEN_MAX_LEN 16        // longest word we accept

/* isBlank		                tells whether a character separates words
Parameters:
char c		                    the character to examine

Returns:
int (bool)	                    1 if c is a space, tab, or end-of-line character */
int isBlank(char c) {
    return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
}

/* splitLine		                breaks a line of text into separate words
Parameters:
char *line				            the line read by fgets
char tokens[][TOKEN_MAX_LEN]		array of word buffers to fill
int maxTokens				        how many words we can hold

Returns:
int	                                the number of words actually found

Notes:
Following the Day 5 advice, read a line with fgets, then process
the string yourself, this walks the character array with an index,
skipping runs of blanks and copying each run of non-blanks into the
next token buffer. Words longer than TOKEN_MAX_LEN-1 are truncated
rather than overflowing the buffer.

Usage:
int n = splitLine(line, tokens, MAX_TOKENS); */
int splitLine(char *line, char tokens[][TOKEN_MAX_LEN], int maxTokens) {
    int count = 0;  // words found so far
    int i = 0;      // where we are in the line

    while ((line[i] != '\0') && (count < maxTokens)) {
        // skip any blanks sitting between words
        while ((line[i] != '\0') && (isBlank(line[i]))) {
            i = i + 1;
        }
        if (line[i] == '\0') {
            break;  // the line ended with trailing blanks
        }
        // copy this word, one character at a time
        int j = 0;
        while ((line[i] != '\0') && (!isBlank(line[i]))) {
            if (j < (TOKEN_MAX_LEN - 1)) {
                tokens[count][j] = line[i];
                j = j + 1;
            }
            i = i + 1;
        }
        tokens[count][j] = '\0'; // strings in C end with a null character
        count = count + 1;
    }
    return (count);
}

/* parseSide		converts the word "BUY" or "SELL" to a side constant
Parameters:
char *word	        the word read from the input line
int *side	        OUT parameter: set to SIDE_BUY or SIDE_SELL

Returns:
int	                0 if the word was valid, -1 if not */ 
int parseSide(char *word, int *side) {
    if (strcmp(word, "BUY") == 0) {
        *side = SIDE_BUY;
        return (0);
    }
    if (strcmp(word, "SELL") == 0) {
        *side = SIDE_SELL;
        return (0);
    }
    return (-1);
} 

/* runFile		        processes every event line in an input file
Parameters:
struct Engine *eng	    the engine to feed events into
char *filename		    path of the input file

Returns:
int	                    0 on success, -1 if the file could not be opened

Notes:
This lowest-level spot prints the error;
main just exits with a nonzero status if we return -1. */
int runFile(struct Engine *eng, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("ERROR: cannot open input file '%s'\n", filename);
        return (-1);
    }

    char line[LINE_MAX_LEN];
    int lineNum = 0;
    while (fgets(line, LINE_MAX_LEN, fp) != NULL) {
        lineNum = lineNum + 1;

        // skip blank lines and # comments
        if ((line[0] == '\n') || (line[0] == '#') || (line[0] == '\0')) {
            continue;
        }

        /* Split the line into whitespace-separated words, then decide
        what to do based on the first word. */
        char tokens[MAX_TOKENS][TOKEN_MAX_LEN];
        int numTokens = splitLine(line, tokens, MAX_TOKENS);
        if (numTokens == 0) {
            continue; // the line was nothing but spaces
        }
        if (tokens[0][0] == '#') {
            continue; // a comment, possibly indented
        }

        int side;

        // LIMIT BUY | SELL <price> <qty>
        if ((strcmp(tokens[0], "LIMIT") == 0) && (numTokens == 4)) {
            if (parseSide(tokens[1], &side) == 0) {
                long price = atoi(tokens[2]);
                long qty = atoi(tokens[3]);
                engineSubmitLimit(eng, side, price, qty);
                continue;
            }
        }

        // MARKET BUY | SELL <qty>
        if ((strcmp(tokens[0], "MARKET") == 0) && (numTokens == 3)) {
            if (parseSide(tokens[1], &side) == 0) {
                long qty = atoi(tokens[2]);
                engineSubmitMarket(eng, side, qty);
                continue;
            }
        }

        // CANCEL BUY | SELL <price> <id>
        if ((strcmp(tokens[0], "CANCEL") == 0) && (numTokens == 4)) {
            if (parseSide(tokens[1], &side) == 0) {
                long price = atoi(tokens[2]);
                long id = atoi(tokens[3]);
                engineCancel(eng, side, price, id);
                continue;
            }
        }

        // BOOK or BOOK <depth>
        if (strcmp(tokens[0], "BOOK") == 0) {
            if (numTokens == 1) {
                bookDisplay(&eng->book, 5);     // default depth
                continue;
            }
            if (numTokens == 2) {
                bookDisplay(&eng->book, atoi(tokens[1]));
                continue;
            }
        }

        printf("WARNING: line %d not understood: %s", lineNum, line);
    }

    fclose(fp);
    return (0);
}


// Mode 2: built-in random-walk simulator

// One remembered resting order, so the simulator can cancel things.
struct SimOrder {
    long id;
    int side;
    long price;
};

#define SIM_REMEMBER 64     // how many recent resting orders we track
#define SIM_START 5000      // starting mid price in ticks
#define SIM_SPREAD 3        /* limit orders are placed within this many
                            ticks of the current mid price */

/* randRange		returns a random integer in [0, n-1]
Parameters:
int n	            the number of possible values

Returns:
int	                a value from 0 to n-1 */
int randRange(int n) {
    return (rand() % n);
}

/* runSim		    generates and processes a random order flow
Parameters:
struct Engine *eng	the engine to feed
long numEvents		how many events to generate
long trend		    -100..100 drift bias (see file comment)

Returns:
nothing (void) */
void runSim(struct Engine *eng, long numEvents, long trend) {
    long mid = SIM_START;
    struct SimOrder remembered[SIM_REMEMBER];
    int rememberedCount = 0;
    int rememberedNext = 0;     // next slot to overwrite (wraps around)
    long i;

    printf("# simulating %ld events, trend %ld%%\n", numEvents, trend);

    for (i = 0; i < numEvents; i++) {
        /* Step the mid price: a random walk with a bias.  
        pick a number 0..99; below the threshold the price ticks up,
        otherwise down.

        trend=0 -> 50/50, trend=100 -> always up. */
        int upChance = 50 + (int)(trend / 2);
        if (randRange(100) < upChance) {
            mid = mid + 1;
        } else {
            mid = mid - 1;
        }

        int roll = randRange(100);
        if (roll < 70) {
            /* 70%: a limit order near the mid. 
            Buys below, sells above, so the book builds up on both 
            sides of the moving price. */
            int side = randRange(2);
            long offset = 1 + randRange(SIM_SPREAD);
            long price;
            if (side == SIDE_BUY) {
                price = mid - offset;
            } else {
                price = mid + offset;
            }
            long qty = 1 + randRange(10);
            long id = engineSubmitLimit(eng, side, price, qty);
            if (id > 0) {
                // remember it so a later event might cancel it
                remembered[rememberedNext].id = id;
                remembered[rememberedNext].side = side;
                remembered[rememberedNext].price = price;
                rememberedNext = (rememberedNext + 1) % SIM_REMEMBER;
                if (rememberedCount < SIM_REMEMBER) {
                    rememberedCount = rememberedCount + 1;
                }
            }
        } else if (roll < 90) {
            // 20%: a market order sweeps into the book
            int side = randRange(2);
            long qty = 1 + randRange(15);
            engineSubmitMarket(eng, side, qty);
        } else {
            /* 10%: try to cancel a remembered order. 
            It may have been filled already, then the cancel simply fails, 
            which is realistic (exchanges reject cancels of filled orders). */
            if (rememberedCount > 0) {
                int pick = randRange(rememberedCount);
                engineCancel(eng, remembered[pick].side,
                             remembered[pick].price, remembered[pick].id);
            }
        }
    }
}


/* main			program entry point

Parameters:
int argc	    number of command-line arguments
char *argv[]	the arguments (argv[0] is the program name)

Returns:
int	            0 on success, 1 on error (the UNIX convention from class) */
int main(int argc, char *argv[]) {
    struct Engine eng;
    engineInit(&eng);

    if ((argc == 2) && (strcmp(argv[1], "--sim") != 0)) {
        // Mode 1: input file
        if (runFile(&eng, argv[1]) != 0) {
            engineFree(&eng);
            exit(1);
        }
    } else if ((argc >= 4) && (strcmp(argv[1], "--sim") == 0)) {
        // Mode 2: simulator
        long numEvents = atoi(argv[2]);
        long trend = atoi(argv[3]);
        if ((numEvents <= 0) || (trend < -100) || (trend > 100)) {
            printf("ERROR: numEvents must be > 0, trend in -100..100\n");
            engineFree(&eng);
            exit(1);
        }
        if (argc >= 5) {
            srand((unsigned int)atoi(argv[4])); // reproducible runs
        } else {
            srand((unsigned int)time(NULL)); // different every run
        }
        runSim(&eng, numEvents, trend);
        bookDisplay(&eng.book, 5);
    } else {
        printf("Usage: %s <eventsFile>\n", argv[0]);
        printf("   or: %s --sim <numEvents> <trend -100..100> [seed]\n",
               argv[0]);
        engineFree(&eng);
        exit(1);
    }

    /* End-of-run report: sizes, heights, and a full invariant check.
    If prices trended, compare the printed height against log2 of the
    level count, a plain BST's height would be near the count itself. */
    printf("\n# end-of-run tree report\n");
    reportTree("BID", eng.book.bids);
    reportTree("ASK", eng.book.asks);

    engineFree(&eng);
    return (0);
}
