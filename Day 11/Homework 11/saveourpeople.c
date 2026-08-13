/* This file picks the set of cities with the largest total population whose
names add up to 200 characters or less.

This is the 0/1 Knapsack problem (Day 11: tabulation):
item        a city
weight      characters in the city's name
value       the city's population
capacity    200 characters

A city is taken or not taken, so greedy is not guaranteed to be optimal.
Instead use tabulation:

best[i][c]      most people savable using only the first i
                cities within c characters

best[i][c]      best[i-1][c]                     (skip city i)
best[i][c]      best[i-1][c-len] + pop           (take city i)
                whichever is larger, if len <= c

The answer is best[n][200]. The chosen cities are recovered by
walking backwards through the table. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSV_PATH     "../../Resources/uscities.csv"
#define MAX_LINE     8192   /* the zips column makes lines very long */
#define MAX_CITIES   200    /* rescue from the 200 largest cities    */
#define MAX_NAME     64     /* longest city name, plus slack         */
#define MAX_CHARS    200    /* the aliens' character limit           */
#define CITY_FIELD   0      /* "city" is the first column            */
#define POP_FIELD    8      /* "population" is the ninth column      */

// A single rescue candidate.
typedef struct city {
    char      name[MAX_NAME];  // the city name          
    int       length;          // weight of the item     
    long long population;      // value of the item      
} City;

/* killNewline
str             a string read with fgets */
void killNewline (char *str) {
    int len = (int) strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len = len - 1;
    }
}

/* getField

Parameters:  
line            one line of the CSV file              
wanted          which field to extract (0 based)              
dest            buffer to copy the field into              
destSize        size of dest, including the '\0'

Returns:     
1 or 0         if the field was found // if there are too few

Fields are quoted and a quoted field may contain a
comma, so strtok is not safe here. Instead, track quote
state. Surrounding quotes are stripped. */
int getField (const char *line, int wanted, char *dest, int destSize) {
    int field   = 0;   // field being scanned    
    int inQuote = 0;   // inside double quotes?  
    int out     = 0;   // characters copied     
    int i       = 0;

    dest[0] = '\0';

    for (i = 0; line[i] != '\0'; i++) {
        char ch = line[i];

        if (ch == '"') {
            inQuote = !inQuote;
        }
        else if (ch == ',' && !inQuote) {
            if (field == wanted) {
                dest[out] = '\0';
                return 1;
            }
            field = field + 1;
            out = 0;
        }
        else if (field == wanted) {
            if (out < destSize - 1) {
                dest[out] = ch;
                out = out + 1;
            }
        }
    }

    if (field == wanted) {
        dest[out] = '\0';
        return 1;
    }
    return 0;
}

/* readCities

Parameters:  
path        CSV file to read
cities      array to fill in
maxCities   how many cities to read at most

Returns:    
the number of cities stored

Skips the header line. The file is already sorted
by population, so the first maxCities data lines are
the largest cities. Empty names and zero populations are skipped.*/
int readCities (const char *path, City cities[], int maxCities) {
    FILE *fp = NULL;
    char  line[MAX_LINE];
    char  name[MAX_NAME];
    char  pop[MAX_NAME];
    int   count = 0;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open '%s'\n", path);
        return 0;
    }

    // throw away the header line 
    if (fgets(line, MAX_LINE, fp) == NULL) {
        fprintf(stderr, "Error: '%s' is empty\n", path);
        fclose(fp);
        return 0;
    }

    while (count < maxCities && fgets(line, MAX_LINE, fp) != NULL) {
        killNewline(line);

        if (!getField(line, CITY_FIELD, name, MAX_NAME)) {
            continue;
        }
        if (!getField(line, POP_FIELD, pop, MAX_NAME)) {
            continue;
        }

        strcpy(cities[count].name, name);
        cities[count].length     = (int) strlen(name);
        cities[count].population = atoll(pop);

        if (cities[count].length > 0 && cities[count].population > 0) {
            count = count + 1;
        }
    }

    fclose(fp);
    return count;
}

/* makeTable

Parameters:  
rows            rows to allocate (cities + 1)             
cols            columns to allocate (characters + 1)

Returns:     
a zeroed rows x cols table, or NULL on failure

Too large for the stack, so it's built on the heap
as an array of row pointers. calloc leaves row 0
all zeros, which is the base case: no cities, no
people saved. */
long long **makeTable (int rows, int cols) {
    long long **table = NULL;
    int i = 0;

    table = malloc(rows * sizeof(long long *));
    if (table == NULL) {
        fprintf(stderr, "Error: out of memory\n");
        return NULL;
    }

    for (i = 0; i < rows; i++) {
        table[i] = calloc(cols, sizeof(long long));
        if (table[i] == NULL) {
            fprintf(stderr, "Error: out of memory at row %d\n", i);
            while (i > 0) {
                i = i - 1;
                free(table[i]);
            }
            free(table);
            return NULL;
        }
    }
    return table;
}

/* freeTable

Parameters:
table           a table made by makeTable
rows            number of rows in that table

Returns:
nothing

Frees every row, then the array of row pointers. */
void freeTable (long long **table, int rows) {
    int i = 0;
    if (table == NULL) {
        return;
    }
    for (i = 0; i < rows; i++) {
        free(table[i]);
    }
    free(table);
}

/* main

Reads the 200 largest cities, fills in the knapsack
table, walks it backwards to recover the chosen
cities, and prints the rescue list. */
int main (void) {
    City        cities[MAX_CITIES];
    long long **best      = NULL;
    int         chosen[MAX_CITIES];
    int         count     = 0;
    int         numChosen = 0;
    int         charsUsed = 0;
    long long   total     = 0;
    int         i         = 0;
    int         c         = 0;

    // read the 200 largest cities 

    count = readCities(CSV_PATH, cities, MAX_CITIES);
    if (count == 0) {
        fprintf(stderr, "Error: no cities read - nobody saved!\n");
        return 1;
    }
    printf("Read %d cities from %s\n\n", count, CSV_PATH);

    // fill in the knapsack table

    best = makeTable(count + 1, MAX_CHARS + 1);
    if (best == NULL) {
        return 1;
    }

    for (i = 1; i <= count; i++) {
        int       len = cities[i - 1].length;
        long long pop = cities[i - 1].population;

        for (c = 0; c <= MAX_CHARS; c++) {
            /* option A: do not rescue city i */
            best[i][c] = best[i - 1][c];

            /* option B: rescue it, if the name still fits */
            if (len <= c && best[i - 1][c - len] + pop > best[i][c]) {
                best[i][c] = best[i - 1][c - len] + pop;
            }
        }
    }

    total = best[count][MAX_CHARS];

    // traverse backwards to find the chosen cities

    c = MAX_CHARS;
    for (i = count; i > 0; i--) {
        /* the value changed, so city i was taken */
        if (best[i][c] != best[i - 1][c]) {
            chosen[numChosen] = i - 1;
            numChosen = numChosen + 1;
            c = c - cities[i - 1].length;
        }
    }

    // report to the aliens

    printf("  SAVE OUR PEOPLE - list for the alien fleet\n");
    printf("Total population saved: %lld\n", total);
    printf("Cities rescued:         %d\n\n", numChosen);

    printf("%-4s %-20s %6s  %s\n", "#", "City", "Chars", "Population");
    printf("---- -------------------- ------  ----------\n");

    // chosen[] was filled in backwards, so print it in reverse
    for (i = numChosen - 1; i >= 0; i--) {
        int index = chosen[i];
        printf("%-4d %-20s %6d  %lld\n",
               numChosen - i,
               cities[index].name,
               cities[index].length,
               cities[index].population);
        charsUsed = charsUsed + cities[index].length;
    }

    printf("---- -------------------- ------  ----------\n");
    printf("%-4s %-20s %6d  %lld\n", "", "TOTAL", charsUsed, total);
    printf("\nCharacters used: %d of %d allowed.\n",
           charsUsed, MAX_CHARS);

    if (charsUsed > MAX_CHARS) {
        fprintf(stderr, "Error: the list is over the limit!\n");
        freeTable(best, count + 1);
        return 1;
    }

    printf("\nThe fleet has its list.  %lld people are saved!\n",
           total);

    // clean up

    freeTable(best, count + 1);
    return 0;
}