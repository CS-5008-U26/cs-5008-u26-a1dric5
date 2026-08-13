/* Task 1:  read city records from uscities.csv into a generic singly-linked
list, then let the user manipulate the list from the console. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE   1000
#define NAME_SIZE     100
#define STATE_SIZE    10
#define HOW_MANY      20                    // how many cities to read

#define DEFAULT_FILE  "../../Resources/uscities.csv"

// Which column of the CSV holds each piece of data.  Counting starts at 0.
#define COL_CITY       0
#define COL_STATE      2
#define COL_POPULATION 8


// The city record taken from Lab 6

typedef struct cityData {
    char name  [NAME_SIZE];
    char state [STATE_SIZE];
    int  population;
} city;


// The generic singly-linked list node

typedef struct singleNode {
    void *data;
    struct singleNode *next;                
} sNode;


// String helpers, carried over from Lab 5

/* killNewline  remove the newline from the end of a string, if there is one

Parameters:
char *str       the string to modify (modified in place)

Returns:
void */

void killNewline (char *str) {
    int len = strlen(str);
    while ((len > 0) && ((str[len-1] == '\n') || (str[len-1] == '\r'))) {
        str[len-1] = '\0';
        len -= 1;
    }
}


/* stripQuotes  remove a matching pair of double quotes around a string

Parameters:
char *str       the string to modify (modified in place)

Returns:
void */

void stripQuotes (char *str) {
    int len = strlen(str);

    if ((len >= 2) && (str[0] == '"') && (str[len-1] == '"')) {
        str[len-1] = '\0';
        for (int i = 0; str[i] != '\0'; i++) {
            str[i] = str[i+1];
        }
    }
}


/* getNextField     copy the next field out of a line, up to the separator

Parameters:
char *start         where to start looking in the line
char separator      the field separator character, like ','
char *out           buffer that receives the field (caller supplies it)

Returns:
char *              pointer to the rest of the line, or NULL if no fields left */

char *getNextField (char *start, char separator, char *out) {

    if (start == NULL) {
        return (NULL);
    }
    if (*start == '\0') {
        return (NULL);
    }

    char *sepPointer = strchr (start, separator);

    if (sepPointer == NULL) {               // this is the last field
        strcpy (out, start);
        stripQuotes (out);
        return (start + strlen(start));
    }

    int howMany = sepPointer - start;
    strncpy (out, start, howMany);
    out[howMany] = '\0';                    // strncpy does not add this
    stripQuotes (out);

    return (sepPointer + 1);
}


/* The six generic list functions.

Every one of them survives a NULL list, a NULL node, or an out-of-range index. 

makeNode        wrap a data pointer in a new list node

Parameters:
void *data      the thing to store (a city *)

Returns:
sNode *         the new node, or NULL if data was NULL or malloc failed

Notes:
The node is on the heap.  Whoever removes it from a list must free it. */

sNode *makeNode (void *data) {
    if (data == NULL) {
        return (NULL);
    }

    sNode *node = malloc (sizeof(sNode));
    if (node == NULL) {
        return (NULL);
    }

    node->data = data;
    node->next = NULL;
    return (node);
}


/* addToFront       make a node the new head of the list
Parameters:
sNode *list         the current head (may be NULL for an empty list)
sNode *node         the node to add

Returns:
sNode *             the new head of the list */

sNode *addToFront (sNode *list, sNode *node) {
    if (node == NULL) {
        return (list);                      // nothing to add
    }

    node->next = list;
    return (node);
}


/* addToEnd     make a node the new tail of the list

Parameters:
sNode *list     the current head (may be NULL for an empty list)
sNode *node     the node to add

Returns:
sNode *         the head of the list (unchanged unless the list was empty)

Traverses the whole list to find the tail, so building a list of n
cities this way costs O(n*n).  Fine for 20 cities. */

sNode *addToEnd (sNode *list, sNode *node) {
    if (node == NULL) {
        return (list);
    }

    node->next = NULL;

    if (list == NULL) {
        return (node);                      // empty list, node becomes the head
    }

    sNode *walker = list;
    while (walker->next != NULL) {
        walker = walker->next;
    }
    walker->next = node;

    return (list);
}


/* getNth       find the n-th node in the list

Parameters:
sNode *list     the head of the list
int n           which node to get - 1 means the first node

Returns:
sNode *         that node, or NULL if n is out of range */

sNode *getNth (sNode *list, int n) {
    if (n < 1) {
        return (NULL);
    }

    sNode *walker = list;
    while ((walker != NULL) && (n > 1)) {
        walker = walker->next;
        n -= 1;
    }

    return (walker);    // NULL if traversed through the end
}


/* deleteNode       unlink a node from the list

Parameters:
sNode *list         the head of the list
sNode *node         the node to remove

Returns:
sNode *             the head of the list, which changes if node WAS the head

This does not free the node, the caller decides. That is what lets
"get" unlink a node and then re-add it instead of destroying it. */

sNode *deleteNode (sNode *list, sNode *node) {
    if ((list == NULL) || (node == NULL)) {
        return (list);
    }

    if (node == list) {                     // deleting the head
        sNode *newHead = list->next;
        node->next = NULL;
        return (newHead);
    }

    sNode *walker = list;
    while ((walker->next != NULL) && (walker->next != node)) {
        walker = walker->next;
    }

    if (walker->next == node) {             // found it
        walker->next = node->next;
        node->next = NULL;
    }

    return (list);                          // node was not in this list
}


/* listLength       count the nodes in the list

Parameters:
sNode *list         the head of the list

Returns:
int                 how many nodes there are (0 for an empty list) */

int listLength (sNode *list) {
    int count = 0;

    sNode *walker = list;
    while (walker != NULL) {
        count += 1;
        walker = walker->next;
    }

    return (count);
}


/* reverseList      reverse the order of the nodes in the list

Parameters:
sNode *list         the head of the list

Returns:
sNode *             the new head (which was the old tail)

Notes:
Re-points the existing nodes; it does not allocate or copy anything.
Three pointers: what came before, where we are, what comes next. */

sNode *reverseList (sNode *list) {
    sNode *previous = NULL;
    sNode *current  = list;

    while (current != NULL) {
        sNode *nextOne = current->next;     // save it before overwriting it
        current->next  = previous;          // point backwards
        previous       = current;
        current        = nextOne;
    }

    return (previous);                      // the old tail
}


/* City handling

makeCity            build a city record on the heap from one CSV line

Parameters:
char *line          one line of the CSV file, newline already removed

Returns:
city *              the new record, or NULL if the line was unusable

Notes:
Walks the fields with getNextField and keeps only the columns named
by COL_CITY, COL_STATE and COL_POPULATION. */

city *makeCity (char *line) {
    char field [BUFFER_SIZE];

    city *newCity = malloc (sizeof(city));
    if (newCity == NULL) {
        return (NULL);
    }

    newCity->name[0]  = '\0';
    newCity->state[0] = '\0';
    newCity->population = 0;

    int whichColumn = 0;
    char *p = line;

    while (p != NULL) {
        p = getNextField (p, ',', field);
        if (p != NULL) {

            if (whichColumn == COL_CITY) {
                strncpy (newCity->name, field, NAME_SIZE - 1);
                newCity->name[NAME_SIZE - 1] = '\0';
            }
            else if (whichColumn == COL_STATE) {
                strncpy (newCity->state, field, STATE_SIZE - 1);
                newCity->state[STATE_SIZE - 1] = '\0';
            }
            else if (whichColumn == COL_POPULATION) {
                newCity->population = atoi (field);
            }

            whichColumn += 1;
        }
    }

    if (newCity->name[0] == '\0') {         // no city name, not a real record
        free (newCity);
        return (NULL);
    }

    return (newCity);
}


/* printCity           print one city record

Parameters:
city *theCity     the record to print

Returns:
void */

void printCity (city *theCity) {
    if (theCity == NULL) {
        printf ("(no city)\n");
        return;
    }

    printf ("%s %s, population %d\n",
            theCity->name, theCity->state, theCity->population);
}


/* readCities       read the first HOW_MANY cities into a linked list

Parameters:
char *filename      the CSV file to read

Returns:
sNode *             the head of the new list, or NULL on failure

Notes:
Cities are added to the tail, so the list keeps the file's order.
The first line of the file is the header row and is skipped. */

sNode *readCities (char *filename) {
    char lineBuffer [BUFFER_SIZE];

    FILE *inFile = fopen (filename, "r");
    if (inFile == NULL) {
        fprintf (stderr, "Could not open the file \"%s\"\n", filename);
        return (NULL);
    }

    sNode *list = NULL;
    int howManySoFar = 0;

    if (fgets (lineBuffer, BUFFER_SIZE, inFile) == NULL) {
        fclose (inFile);                    // the file was empty
        return (NULL);
    }

    while ((howManySoFar < HOW_MANY) &&
           (fgets (lineBuffer, BUFFER_SIZE, inFile) != NULL)) {

        killNewline (lineBuffer);

        city *newCity = makeCity (lineBuffer);
        if (newCity != NULL) {
            list = addToEnd (list, makeNode (newCity));
            howManySoFar += 1;
        }
    }

    fclose (inFile);
    return (list);
}


/* freeList     free every node in the list, and the city inside each one

Parameters:
sNode *list     the head of the list

Returns:
void */

void freeList (sNode *list) {
    while (list != NULL) {
        sNode *nextOne = list->next;        // save it before we free the node
        free (list->data);
        free (list);
        list = nextOne;
    }
}


/*The console loop

askForNumber        prompt for a number and read it from the console

Parameters:
none

Returns:
int                 the number typed, or 0 if the input was not usable */

int askForNumber (void) {
    char buffer [BUFFER_SIZE];

    printf ("Enter a number: ");
    if (fgets (buffer, BUFFER_SIZE, stdin) == NULL) {
        return (0);
    }
    killNewline (buffer);

    return (atoi (buffer));
}


int main (int argc, char **argv) {
    char command [BUFFER_SIZE];

    char *filename = DEFAULT_FILE;
    if (argc >= 2) {
        filename = argv[1];
    }

    sNode *list = readCities (filename);
    if (list == NULL) {
        fprintf (stderr, "No cities were read.\n");
        return (1);
    }

    int keepGoing = 1;
    while (keepGoing) {

        printf ("size, delete, reverse, get, or print: ");
        if (fgets (command, BUFFER_SIZE, stdin) == NULL) {
            break;                          // end of input
        }
        killNewline (command);

        if (strcmp (command, "size") == 0) {
            printf ("Size is %d\n", listLength (list));
        }

        else if (strcmp (command, "delete") == 0) {
            int n = askForNumber ();
            sNode *victim = getNth (list, n);
            if (victim == NULL) {
                printf ("There is no city number %d\n", n);
            }
            else {
                list = deleteNode (list, victim);
                free (victim->data);        // the city record
                free (victim);              // and the node itself
            }
        }

        else if (strcmp (command, "reverse") == 0) {
            list = reverseList (list);
        }

        else if (strcmp (command, "get") == 0) {
            int n = askForNumber ();
            sNode *found = getNth (list, n);
            if (found == NULL) {
                printf ("There is no city number %d\n", n);
            }
            else {
                list = deleteNode (list, found);    // unlink, do not free
                list = addToFront (list, found);    // put it back at the head
            }
        }

        else if (strcmp (command, "print") == 0) {
            int n = askForNumber ();
            sNode *walker = list;
            while ((walker != NULL) && (n > 0)) {
                printCity ((city *) walker->data);
                walker = walker->next;
                n -= 1;
            }
        }

        else {
            keepGoing = 0;                  // anything else stops the program
        }
    }

    freeList (list);
    return (0);
}