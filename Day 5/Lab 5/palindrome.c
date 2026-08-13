/* This filw does the following: 

Task 1: read lines from the console and echo them until a bare ENTER.
Task 2: report the length of each line, not counting the newline.
Task 3: condition a line, keep only the letters, forced to lower case.
Task 4: recognize palindromes; echo only the lines that are palindromes.
Task 7: if a filename is given on the command line, read that file
        instead of the console.*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 200
#define TRUE  1
#define FALSE 0


/* killNewline      remove the newline from the end of a string, if there is one
Parameters:
char *str           the string to modify (modified in place)

Returns:
void */

void killNewline (char *str) {
    int len = strlen(str);
    while ((len > 0) && ((str[len-1] == '\n') || (str[len-1] == '\r'))) {
        str[len-1] = '\0';
        len -= 1;
    }
}


/* condition      copy a string keeping only its letters, forced to lower case

Parameters:
char *in     the string to read from
char *out    buffer that receives the conditioned string (caller supplies)

Returns:
void

Notes:
The result is never longer than the input, so an out buffer the same
size as the in buffer is always big enough. */

void condition (char *in, char *out) {
    for ( ; *in; in++) {
        if (isalpha(*in)) {
            *out = tolower(*in);
            out++;
        }
    }
    *out = '\0';                        // terminate the output string
}


/* palindrome       tell whether a string reads the same forwards and backwards

Parameters:
char *in            the string being examined (it is conditioned first)

Returns:
int (bool)          TRUE if in is a palindrome, FALSE if it isn't */

int palindrome (char *in) {
    char conditioned [BUFFER_SIZE];

    condition (in, conditioned);

    if (conditioned[0] == '\0') {
        return (FALSE);                 // no letters at all
    }

    int left  = 0;
    int right = strlen(conditioned) - 1;

    while ((left < right) && (conditioned[left] == conditioned[right])) {
        left  += 1;
        right -= 1;
    }

    return (left >= right);
}


/* processConsole    prompt for lines and echo the ones that are palindromes

Parameters:
none

Returns:
void

Note:
Stops when the user presses ENTER on an empty line, or at end of input.*/

void processConsole (void) {
    char inputBuffer [BUFFER_SIZE];

    inputBuffer[0] = 'X';               // enter the loop at least once

    while (inputBuffer[0] != '\n') {

        printf ("Enter a string: ");

        if (fgets (inputBuffer, BUFFER_SIZE, stdin) == NULL) {
            break;                      // end of input (Ctrl-D), stop
        }

        if (inputBuffer[0] != '\n') {
            killNewline (inputBuffer);
            if (palindrome (inputBuffer)) {
                printf ("%s\n", inputBuffer);
            }
        }
    }
}


/* processFile      print every line of a file that is a palindrome

Parameters:
char *filename      the name of the file to read

Returns:
int                 0 if successful, 1 if the file could not be opened.*/

int processFile (char *filename) {
    char inputBuffer [BUFFER_SIZE];

    FILE *inFile = fopen (filename, "r");
    if (inFile == NULL) {
        fprintf (stderr, "Could not open the file \"%s\"\n", filename);
        return (1);
    }

    while (fgets (inputBuffer, BUFFER_SIZE, inFile) != NULL) {
        killNewline (inputBuffer);
        if (palindrome (inputBuffer)) {
            printf ("%s\n", inputBuffer);
        }
    }

    fclose (inFile);
    return (0);
}


int main (int argc, char **argv) {

    if (argc < 2) {
        processConsole ();
        return (0);
    }

    return (processFile (argv[1]));
}