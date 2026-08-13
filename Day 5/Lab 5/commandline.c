/* This file prints each command line arg on its own line. 

If a filename is given, print the contents of that file instead.*/


#include <stdio.h>

#define BUFFER_SIZE 200


int main (int argc, char **argv) {
    char inputBuffer [BUFFER_SIZE];

    if (argc <= 1) {
        return (0);                     // no args, nothing to do
    }

    FILE *inFile = fopen (argv[1], "r");
    if (inFile == NULL) {
        fprintf (stderr, "Could not open the file \"%s\"\n", argv[1]);
        return (1);
    }

    while (fgets (inputBuffer, BUFFER_SIZE, inFile) != NULL) {
        printf ("%s", inputBuffer);     // fgets kept the newline, don't add one
    }

    fclose (inFile);
    return (0);
}