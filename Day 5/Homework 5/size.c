/* prints the size of a file in bytes. */

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 1000


/* killNewline, strips a trailing '\n' from a string, in place.
 
Needed because fgets keeps the newline. 
Without this, stat() would be handed "uscities.csv\n", a filename that 
does not exist, and you would get a confusing ENOENT on a file that is 
sitting right there. */
void killNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


/* reportStatError, explains why stat() failed, using errno.
errno is a global that the system sets when a call fails. 

It only means anything if the call failed, so it's only read inside the error branch. */
void reportStatError(char *filename) {
    switch (errno) {
        case ENOENT:
            printf("No file as %s\n", filename);
            break;
        case EACCES:
            printf("No permission to read %s\n", filename);
            break;
        case ENOTDIR:
            printf("Part of the path is not a directory: %s\n", filename);
            break;
        case EIO:
            printf("I/O error trying to read %s\n", filename);
            break;
        default:
            printf("Couldn't get info about %s\n", filename);
            break;
    }
}


/* main, declared to receive the command-line args.
argc, how many arguments there are
argv, array of strings holding them

argv[0] is always the command name itself, so the first real argument is argv[1]. 
That is why "one argument was supplied" is argc == 2, not argc == 1. 
This off-by-one catches everybody once.
*/
int main(int argc, char *argv[]) {
    char filenameBuffer[BUFSIZE];
    char *filename;

    if (argc >= 2) {
        /* The user gave us the filename on the command line. 
        argv[1] is already a proper string with no newline attached, 
        the shell handled that, so no killNewline is needed on this path. */
        filename = argv[1];

    } else {
        /* No argument, so ask. 
        REFERENCE: fgets into a buffer pattern from Day 5 slide 63.*/
        printf("Enter the name of a file: ");

        if (fgets(filenameBuffer, BUFSIZE, stdin) == NULL) {
            printf("No filename entered.\n");
            return 1;
        }
        killNewline(filenameBuffer);

        filename = filenameBuffer;
    }

    /* Asks the file system about this name 
    
    stat() fills in the struct, which is why need to pass its
    address with &. 
    
    REFERENCE: (Day 5 slide 18) -- "changing the value of a parameter"
    the function cannot hand back a whole struct through its return value, 
    so it takes a pointer and writes through it. The return value is reserved 
    for success or failure.
    */
    struct stat fileStat;

    if (stat(filename, &fileStat) < 0) {
        reportStatError(filename);
        return 1;
    }

    /* A directory has a size too, but it is not what anyone means
    
    S_ISREG asks "is this an ordinary file?"
    
    Without this check, ./size ../Resources would report a few hundred bytes 
    of directory bookkeeping, which is a confusing answer rather than a wrong one. */
    if (!S_ISREG(fileStat.st_mode)) {
        if (S_ISDIR(fileStat.st_mode)) {
            printf("%s is a directory, not a file\n", filename);
        } else {
            printf("%s is not a regular file\n", filename);
        }
        return 1;
    }

    int fileSize = fileStat.st_size;

    printf("%s has %d bytes\n", filename, fileSize);

    return 0;
}