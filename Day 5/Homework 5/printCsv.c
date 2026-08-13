/* This file reads a CSV file and print each line, then each field
of that line indented beneath it, with >brackets< around everything so
stray whitespace is visible. */

#include <stdio.h>
#include <string.h>

#define BUFSIZE 10000



/* strips a trailing '\n' from a string, in place.
 
 fgets keeps the newline it read. A line from the file arrives as
 'a' ',' 'b' ',' 'c' '\n' '\0'
 and if I print that inside brackets i get ">a,b,c\n<". The bracket
 lands on the next line. 
 
 Overwriting the '\n' with '\0' ends the string
 one character earlier, so the newline is no longer part of it.

 length guard usage: strlen("") is 0, so str[strlen(str) - 1] would be
 str[-1], a read before the start of the array. */
void killNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


/* getNextField pulls one field out of a line.
 
 char start, pointer to wherever we are in the line
 char separator, the dividing character, ','
 char out, caller's buffer; the field text is written here
 
returns, pointer to the character after the separator, so the 
caller can feed it back in for the next field; or NULL when there 
is no field left to read.
 
the two value design is the key idea: the field goes out through the
`out` parameter, and the position is what comes back as the return value.*/
char *getNextField(char *start, char separator, char *out) {

    /* acts as a guard, checks to see if reached the end of the line
     
     This'll stops the caller's loop.
     It fires only when we are on the '\0'. 
     A field that is empty but valid as in the middle field of "a,,c"
     has start pointing at ',', not '\0', so it correctly produces an 
     empty field rather than ending the line.*/
    if (*start == '\0') {
        return NULL;
    }

    /* finds the next separator 
     
     strchr scans forward from `start` and hands back the address of the
     first ',' it finds, or NULL if there is none left in this string. */
    char *sepPointer = strchr(start, separator);
    char *result;

    if (sepPointer != NULL) {
        /* a field followed by a comma 
         
         Pointer subtraction gives a count, not an address. If start is
         at 'a' and sepPointer is at the ',' two cells later, then
         sepPointer - start == 2, which is exactly how many characters
         belong to this field. */
        int fieldLen = sepPointer - start;

        strncpy(out, start, fieldLen);

        /* strncpy does not append '\0' when it copies exactly fieldLen
        characters, it only pads with '\0' if the source ran short.
          
        Need to terminate the string ourselves. 
         
         Skip this line and `out` keeps whatever garbage was in it 
         from the previous field, and it'll receive an output like 
         ">a2<" then ">b22<". */
        out[fieldLen] = '\0';

        /* Hand back the position just past the comma. */
        result = sepPointer + 1;

    } else {
        /* Last field on the line: no comma after it
        
        Everything from start to the end of the string is the field, so
        a plain strcpy does it (and strcpy does add the '\0'). */
        strcpy(out, start);

        /* Return a pointer to the '\0' at the end. The caller sees a
        non-null value, prints this field, and calls once more,
        that next call hits the guard at the top and returns NULL,
        ending the loop. One extra call, but the logic stays simple. */
        result = start + strlen(start);
    }

    /* unwraps a field that was enclosed in double quotes 
     
     after the field is already isolated in `out`, so it applies 
     identically to both cases above.
     
     The length must be at least 2, otherwise a lone '"' would look like
     both the opening and closing quote to the two tests below. */
    int outLen = strlen(out);
    if (outLen >= 2 && out[0] == '"' && out[outLen - 1] == '"') {

        /* Drop the closing quote by ending the string one char earlier. */
        out[outLen - 1] = '\0';

        /* shift everything left by one to drop the opening quote.

        cannot just return `out + 1`, `out` is the caller's buffer
        and the caller reads from `out` itself, so the characters have
        to physically move.
        
        `src` starts one past the opening quote. The loop stops when
        *src is the '\0' seen on the line above, so the quote at
        the end is never copied. */
        char *src = out + 1;
        char *dst = out;
        while (*src) {
            *dst = *src;
            dst++;
            src++;
        }
        *dst = '\0';
    }

    return result;
}


int main(void) {
    char filename[BUFSIZE];
    char lineBuffer[BUFSIZE];
    char fieldBuffer[BUFSIZE];

    /* STEP A: get the filename from the console */
    printf("Enter the file name: ");

    if (fgets(filename, BUFSIZE, stdin) == NULL) {
        printf("No filename entered.\n");
        return 1;
    }
    killNewline(filename);   // strip the newline fgets left behind

    /* STEP B: open the file */
    FILE *inFile = fopen(filename, "r");

    if (inFile == NULL) {
        printf("Could not open file: %s\n", filename);
        return 1;
    }

    /* STEP B: read the file one line at a time 
     
    fgets returns the buffer address on success, NULL once there is
    nothing left to read. 
     
    convert that into a loop.
     */
    int keepReading = 1;
    while (keepReading) {
        char *whatWasRead = fgets(lineBuffer, BUFSIZE, inFile);
        keepReading = (whatWasRead != NULL);

        if (keepReading) {
            killNewline(lineBuffer);
            printf(">%s<\n", lineBuffer);

            /* STEP C: traverse the line field by field 
            
            p is the cursor. It starts at the front of the line, and
            each call to getNextField moves it past one more field.
            When nothing is left, getNextField returns NULL and this
            inner loop ends, then we go read the next line.
             */
            char *p = lineBuffer;
            while (p != NULL) {
                p = getNextField(p, ',', fieldBuffer);
                if (p != NULL) {
                    printf("    >%s<\n", fieldBuffer);
                }
            }
        }
    }

    fclose(inFile);
    return 0;
}