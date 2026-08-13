/* 
Task 1:  read candy-data.csv into an array of heap-allocated structs,
          close the file, then print the name of every candy.
Task 2:  list the chocolate candies - upper case if the candy also has
          caramel, lower case if it does not - and report what percent of
          the chocolate candies have caramel.
Task 3:  for each of the nine attributes, report the average sugar, price
          and win percents of the candies having that attribute.  Then do
          the same for the candies above the average sugar percent, and
          for the candies above the average price percent. */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE      1000       // longest line we can read from the file
#define MAX_CANDIES      200        // most candies we can hold
#define NUM_FIELDS       13         // columns in the CSV
#define NUM_ATTRIBUTES   9          // fields 2 through 10 are the yes/no ones

#define FIELD_SUGAR      0          // which value getValue should hand back
#define FIELD_PRICE      1
#define FIELD_WIN        2


/* Candy    one row of candy-data.csv

The nine yes/no columns are stored as ints, 1 for yes and 0 for no.
The three percent columns are stored as doubles exactly as the file
holds them, sugar and price are fractions from 0 to 1, win is 0 to 100.

The name points at its own heap block, so freeCandy has to free both
the name and the struct. */

typedef struct candy {
    char   *name;
    int     chocolate;
    int     fruity;
    int     caramel;
    int     peanutyAlmondy;
    int     nougat;
    int     crispedRiceWafer;
    int     hard;
    int     bar;
    int     pluribus;
    double  sugarPercent;
    double  pricePercent;
    double  winPercent;
} Candy;


/* attributeNames    printable name of each yes/no field, in column order

The order here has to match the order getAttribute uses. */

char *attributeNames [NUM_ATTRIBUTES] = {
    "chocolate",
    "fruity",
    "caramel",
    "peanuty/almondy",
    "nougat",
    "crisped rice/wafer",
    "hard",
    "bar",
    "pluribus"
};


/* killNewline      remove the newline from the end of a string, if there is one

Parameters:
char *str           the string to modify (modified in place)

fgets leaves the '\n' in the buffer.  
A file saved on Windows or out of Excel can also leave a '\r' behind, so strip 
either one from the end. */

void killNewline (char *str) {
    int len = strlen(str);
    while ((len > 0) && ((str[len-1] == '\n') || (str[len-1] == '\r'))) {
        str[len-1] = '\0';
        len -= 1;
    }
}


/* stripQuotes      remove a matching pair of double quotes around a string

Parameters:
char *str           the string to modify (modified in place)

Kills the closing quote, then slides the rest of the string left one
position over the opening quote. 
Nothing happens unless the very first and very last characters are both quotes. */

void stripQuotes (char *str) {
    int len = strlen(str);

    if ((len >= 2) && (str[0] == '"') && (str[len-1] == '"')) {
        str[len-1] = '\0';
        for (int i = 0; str[i] != '\0'; i++) {
            str[i] = str[i+1];
        }
    }
}


/* getNextField        copy the next field out of a line, up to the separator

Parameters:
char *start         where to start looking in the line
char separator      the field separator character, e.g. ','
char *out           buffer that receives the field (caller supplies it)

Returns:
char *              pointer to the rest of the line (past the separator),
                    or NULL if there are no more fields */

char *getNextField (char *start, char separator, char *out) {

    // Special case #1: already at the end of the string, no fields left
    if (*start == '\0') {
        return (NULL);
    }

    char *sepPointer = strchr (start, separator);

    // Special case #2: no separator left, so this is the last field
    if (sepPointer == NULL) {
        strcpy (out, start);
        stripQuotes (out);
        return (start + strlen(start));     // points at the end-of-string
    }

    // Normal case: copy the characters between start and the separator
    int howMany = sepPointer - start;
    strncpy (out, start, howMany);
    out[howMany] = '\0';                    // strncpy does not add this
    stripQuotes (out);

    return (sepPointer + 1);                // the rest of the line
}


/* copyToHeap       make a heap copy of a string

Parameters:
char *str           the string to copy

Returns:
char *              a new heap block holding the same characters,
                    or NULL if the allocation failed

Room for the end-of-string character is why this is strlen + 1.
The caller owns the block and has to free it. */

char *copyToHeap (char *str) {
    char *copy = malloc (strlen(str) + 1);

    if (copy != NULL) {
        strcpy (copy, str);
    }

    return (copy);
}


/* makeCandy        build one heap Candy from one line of the CSV

Parameters:
char *line          the line to parse (this function does not modify it)

Returns:
Candy *             a new heap Candy, or NULL if the line was unusable


A line has to have all 13 fields. Anything shorter is skipped rather 
than half-filled, so the caller never sees a partial candy. */

Candy *makeCandy (char *line) {
    char nextField [BUFFER_SIZE];
    char *values [NUM_FIELDS];
    int count = 0;

    /* Pull the fields out of the line, copying each one to the heap so it
    // outlives the caller's buffer. */
    char *p = line;
    while ((p != NULL) && (count < NUM_FIELDS)) {
        p = getNextField (p, ',', nextField);
        if (p != NULL) {
            values[count] = copyToHeap (nextField);
            count += 1;
        }
    }

    if (count < NUM_FIELDS) {
        for (int i = 0; i < count; i++) {   // don't leak the partial row
            free (values[i]);
        }
        return (NULL);
    }

    Candy *candy = malloc (sizeof(Candy));
    if (candy == NULL) {
        for (int i = 0; i < count; i++) {
            free (values[i]);
        }
        return (NULL);
    }

    candy->name             = values[0];    // already on the heap, just keep it
    candy->chocolate        = atoi (values[1]);
    candy->fruity           = atoi (values[2]);
    candy->caramel          = atoi (values[3]);
    candy->peanutyAlmondy   = atoi (values[4]);
    candy->nougat           = atoi (values[5]);
    candy->crispedRiceWafer = atoi (values[6]);
    candy->hard             = atoi (values[7]);
    candy->bar              = atoi (values[8]);
    candy->pluribus         = atoi (values[9]);
    candy->sugarPercent     = atof (values[10]);
    candy->pricePercent     = atof (values[11]);
    candy->winPercent       = atof (values[12]);

    for (int i = 1; i < NUM_FIELDS; i++) {  // start at 1, the name is kept
        free (values[i]);
    }

    return (candy);
}


/* freeCandy        give back everything one candy owns

Parameters:
Candy *candy        the candy to free

Returns:
void

The name has to go first. Free the struct and the name pointer inside
it is gone with it. */

void freeCandy (Candy *candy) {
    if (candy != NULL) {
        free (candy->name);
        free (candy);
    }
}


/* readCandyFile    read a whole candy file into an array of Candy pointers

Parameters:
char *fileName      the file to open
Candy *candies[]    array that receives the pointers (caller supplies it)
int maxCandies      how many pointers the array can hold

Returns:
int                 how many candies were read, or -1 if the file would
                    not open
Notes:
The first line of the file is the column names and is thrown away.
The file is closed before this function returns. */

int readCandyFile (char *fileName, Candy *candies[], int maxCandies) {
    char buffer [BUFFER_SIZE];
    int count = 0;

    FILE *inFile = fopen (fileName, "r");
    if (inFile == NULL) {
        return (-1);
    }

    // Throw away the header line
    if (fgets (buffer, BUFFER_SIZE, inFile) == NULL) {
        fclose (inFile);
        return (0);                         // empty file, no candies
    }

    while (fgets (buffer, BUFFER_SIZE, inFile) != NULL) {
        killNewline (buffer);

        if (buffer[0] != '\0') {            // skip blank lines
            Candy *candy = makeCandy (buffer);

            if (candy == NULL) {
                fprintf (stderr, "Skipping unreadable line: %s\n", buffer);
            } else if (count >= maxCandies) {
                fprintf (stderr, "Too many candies, stopping at %d\n", maxCandies);
                freeCandy (candy);
                break;
            } else {
                candies[count] = candy;
                count += 1;
            }
        }
    }

    fclose (inFile);
    return (count);
}


/* getAttribute     read one of the nine yes/no fields by number

Parameters:
Candy *candy        the candy to look at
int which           0 for chocolate through 8 for pluribus

Returns:
int                 1 if the candy has that attribute, 0 if it does not


This is what lets Task 3 loop over the attributes instead of writing
the same block of code nine times. The order matches attributeNames. */

int getAttribute (Candy *candy, int which) {
    switch (which) {
        case 0:  return (candy->chocolate);
        case 1:  return (candy->fruity);
        case 2:  return (candy->caramel);
        case 3:  return (candy->peanutyAlmondy);
        case 4:  return (candy->nougat);
        case 5:  return (candy->crispedRiceWafer);
        case 6:  return (candy->hard);
        case 7:  return (candy->bar);
        case 8:  return (candy->pluribus);
        default: return (0);
    }
}


/* getValue         read one of the three percent fields by number

Parameters:
Candy *candy        the candy to look at
int field           FIELD_SUGAR, FIELD_PRICE or FIELD_WIN

Returns:
double              that candy's value for the field, 0.0 if field is bad */

double getValue (Candy *candy, int field) {
    switch (field) {
        case FIELD_SUGAR:  return (candy->sugarPercent);
        case FIELD_PRICE:  return (candy->pricePercent);
        case FIELD_WIN:    return (candy->winPercent);
        default:           return (0.0);
    }
}


/* average          average one field over a group of candies

Parameters:
Candy *group[]      the candies to average over
int count           how many candies are in the group
int field           FIELD_SUGAR, FIELD_PRICE or FIELD_WIN

Returns:
double              the average, or 0.0 if the group is empty */

double average (Candy *group[], int count, int field) {
    double total = 0.0;

    if (count <= 0) {
        return (0.0);
    }

    for (int i = 0; i < count; i++) {
        total += getValue (group[i], field);
    }

    return (total / count);
}


/* selectAttribute      collect the candies that have a given attribute

Parameters:
Candy *all[]            every candy
int count               how many candies are in all[]
int which               0 through 8, the attribute to test
Candy *out[]            array that receives the matches (caller supplies it)

Returns:
int                     how many candies matched

Only the pointers are copied. Both arrays point at the same structs,
so out[] must never be freed, all[] owns them. */

int selectAttribute (Candy *all[], int count, int which, Candy *out[]) {
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (getAttribute (all[i], which)) {
            out[found] = all[i];
            found += 1;
        }
    }

    return (found);
}


/* selectAbove      collect the candies above a cutoff on one field

Parameters:
Candy *all[]        every candy
int count           how many candies are in all[]
int field           FIELD_SUGAR, FIELD_PRICE or FIELD_WIN
double cutoff       the value to beat
Candy *out[]        array that receives the matches (caller supplies it)

Returns:
int                 how many candies matched

Strictly greater than the cutoff, so a candy sitting exactly on the
average is left out. */

int selectAbove (Candy *all[], int count, int field, double cutoff, Candy *out[]) {
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (getValue (all[i], field) > cutoff) {
            out[found] = all[i];
            found += 1;
        }
    }

    return (found);
}


/* printSummary     print one row of the Task 3 table

Parameters:
char *label         what to call this group
Candy *group[]      the candies in the group
int count           how many candies are in the group

Returns:
void */

void printSummary (char *label, Candy *group[], int count) {
    printf ("%-22s %5d   %8.3f   %8.3f   %8.2f\n",
            label,
            count,
            average (group, count, FIELD_SUGAR),
            average (group, count, FIELD_PRICE),
            average (group, count, FIELD_WIN));
}


/* printSummaryHeading  print the column titles for the Task 3 table

Returns:
void */

void printSummaryHeading (void) {
    printf ("%-22s %5s   %8s   %8s   %8s\n",
            "attribute", "count", "sugar", "price", "win");
    printf ("---------------------------------------------------------------\n");
}


/* printUpper       print a string forced to upper case
Parameters:
char *str           the string to print

Returns:
void

Prints a character at a time so the stored name is left untouched -
nothing on the heap gets modified just to change how it looks. */

void printUpper (char *str) {
    for ( ; *str; str++) {
        putchar (toupper(*str));
    }
}


/* printLower     print a string forced to lower case
Parameters:
char *str    the string to print

Returns:
void */

void printLower (char *str) {
    for ( ; *str; str++) {
        putchar (tolower(*str));
    }
}


// main    run all three tasks against the candy file

int main (void) {
    char fileName [BUFFER_SIZE];
    Candy *candies [MAX_CANDIES];
    Candy *group   [MAX_CANDIES];       

    printf ("Enter the file name: ");
    if (fgets (fileName, BUFFER_SIZE, stdin) == NULL) {
        printf ("No file name was entered.\n");
        return (1);
    }
    killNewline (fileName);

    int numCandies = readCandyFile (fileName, candies, MAX_CANDIES);

    if (numCandies < 0) {
        printf ("Could not open the file \"%s\"\n", fileName);
        return (1);
    }

    if (numCandies == 0) {
        printf ("No candies were found in \"%s\"\n", fileName);
        return (1);
    }

    // Task 1: print every candy 

    printf ("\nAll %d candies:\n", numCandies);
    for (int i = 0; i < numCandies; i++) {
        printf ("    %s\n", candies[i]->name);
    }

    // Task 2: the chocolate candies 

    printf ("\nChocolate candies (UPPER CASE means it also has caramel):\n");

    int numChocolate = 0;
    int numBoth = 0;

    for (int i = 0; i < numCandies; i++) {
        if (candies[i]->chocolate) {
            numChocolate += 1;

            printf ("    ");
            if (candies[i]->caramel) {
                numBoth += 1;
                printUpper (candies[i]->name);
            } else {
                printLower (candies[i]->name);
            }
            printf ("\n");
        }
    }

    if (numChocolate == 0) {
        printf ("    (none)\n");
    } else {
        printf ("\n%d of the %d chocolate candies have caramel (%.1f%%)\n",
                numBoth, numChocolate, (100.0 * numBoth) / numChocolate);
    }

    // Task 3: summary by attribute 

    printf ("\nAverages by attribute:\n");
    printSummaryHeading ();

    for (int which = 0; which < NUM_ATTRIBUTES; which++) {
        int found = selectAttribute (candies, numCandies, which, group);
        printSummary (attributeNames[which], group, found);
    }

    // Task 3: above-average sugar and price 

    double avgSugar = average (candies, numCandies, FIELD_SUGAR);
    double avgPrice = average (candies, numCandies, FIELD_PRICE);

    printf ("\nAverages over all %d candies:   sugar %.3f   price %.3f   win %.2f\n",
            numCandies, avgSugar, avgPrice,
            average (candies, numCandies, FIELD_WIN));

    printf ("\nAverages for the candies above those cutoffs:\n");
    printSummaryHeading ();

    int aboveSugar = selectAbove (candies, numCandies, FIELD_SUGAR, avgSugar, group);
    printSummary ("above avg sugar", group, aboveSugar);

    int abovePrice = selectAbove (candies, numCandies, FIELD_PRICE, avgPrice, group);
    printSummary ("above avg price", group, abovePrice);

    // give the heap back 

    for (int i = 0; i < numCandies; i++) {
        freeCandy (candies[i]);
    }

    return (0);
}