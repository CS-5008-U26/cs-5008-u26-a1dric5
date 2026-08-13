/* reads uscities.csv, skip the header line, then process the next 20 
city records. 

Print the total population of those 20 cities and the name of the northernmost one.

killNewline and getNextField are the same functions from printCsv.c.
They are copied in here so cities.c is a single self contained file.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>      // uses atoi and atof

#define BUFSIZE        5000
#define CITIES_WANTED  20

/* Which field holds what. 
The assignment numbers columns from 1, but the loop counter starts at 0, 
so each of these is one less than the handout's number. 
Naming them stops that off by one from hiding inside a bare number in 
the middle of the loop. */
#define FIELD_CITY_NAME  1     // column 2, the ASCII city name 
#define FIELD_LATITUDE   6     // column 7 
#define FIELD_POPULATION 8     // column 9 


/* killNewline, strips a trailing '\n' from a string, in place.
 
fgets keeps the newline it read, so without this the last field on
every line would carry a '\n' into atoi/atof.
 
The length guard matters: strlen("") is 0, so str[strlen(str) - 1]
would be str[-1], a read before the start of the array. */
void killNewline(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}


/* getNextField, pulls one field out of a line.

start, pointer to wherever we are in the line

separator, the dividing character, ','

out, caller's buffer; the field text is written here
 
returns, pointer to the character after the separator, or NULL
when there is no field left to read. */
char *getNextField(char *start, char separator, char *out) {

    /* Already sitting on the end of string means no field is left. */
    if (*start == '\0') {
        return NULL;
    }

    char *sepPointer = strchr(start, separator);
    char *result;

    if (sepPointer != NULL) {
        /* Pointer subtraction gives a count: 
        how many characters lie between where we are and the next comma. */
        int fieldLen = sepPointer - start;

        strncpy(out, start, fieldLen);

        /* strncpy does not add '\0' when it copies exactly fieldLen
        characters, so we place it ourselves. */
        out[fieldLen] = '\0';

        result = sepPointer + 1;

    } else {
        // No comma left, this is the last field on the line.
        strcpy(out, start);
        result = start + strlen(start);
    }

    /* Unwraps a field that was enclosed in double quotes. 
    uscities.csv quotes its text fields, without it the city name would print with quotes. */
    int outLen = strlen(out);
    if (outLen >= 2 && out[0] == '"' && out[outLen - 1] == '"') {

        out[outLen - 1] = '\0';        // drop the closing quote

        /* Shift everything left by one to drop the opening quote. 
        Same two pointer copy loop as removeCommas from class. */
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

    // Running totals across all 20 cities
    int    totalPopulation   = 0;
    double highestLatitude   = 0.0;
    char   northernmostCity[BUFSIZE];
    int    citiesRead        = 0;

    northernmostCity[0] = '\0';   // empty until we see the first city

    // Get the filename 
    printf("Enter the file name: ");

    if (fgets(filename, BUFSIZE, stdin) == NULL) {
        printf("No filename entered.\n");
        return 1;
    }
    killNewline(filename);

    /* Opens it. 
    
    Steve Rule: ALWAYS check for NULL */
    FILE *inFile = fopen(filename, "r");

    if (inFile == NULL) {
        printf("Could not open file: %s\n", filename);
        return 1;
    }

    /* Throw away the header line 
    
    The first line names the columns rather than describing a city.
    Reads and does nothing with it.

    Reading it advances the file position past that line so the loop 
    below starts on real data. */
    if (fgets(lineBuffer, BUFSIZE, inFile) == NULL) {
        printf("File is empty: %s\n", filename);
        fclose(inFile); 
        return 1;
    }

    /* Process the next 20 lines 

    Two ways to stop: 
    - collected 20 cities 
    - the file ran out early
    
    Both conditions live in the while test, so the loop
    body never has to worry about either one. */
    while (citiesRead < CITIES_WANTED
           && fgets(lineBuffer, BUFSIZE, inFile) != NULL) {

        killNewline(lineBuffer);

        /* Per line values, reset each time through. */
        char   cityName[BUFSIZE];
        double latitude   = 0.0;
        int    population = 0;
        int    fieldIndex = 0;

        cityName[0] = '\0';

        /* Traverse field by field, keeping only the three needed.

        Need to count every field during traversal, including the ones
        ignored, or the indices would not line up.
         */
        char *p = lineBuffer;
        while (p != NULL) {
            p = getNextField(p, ',', fieldBuffer);

            if (p != NULL) {
                if (fieldIndex == FIELD_CITY_NAME) {
                    /* Copy it out. 
                    
                    fieldBuffer gets overwritten by the very next call, 
                    so keeping a pointer to it would leave holding the 
                    longitude by the end of the line. */
                    strcpy(cityName, fieldBuffer);

                } else if (fieldIndex == FIELD_LATITUDE) {
                    /* atof: string to double. 
                    Latitudes have a decimal point, so atoi would 
                    truncate 47.62 to 47 and make several cities look tied. */
                    latitude = atof(fieldBuffer);

                } else if (fieldIndex == FIELD_POPULATION) {
                    // atoi: string to int. Populations are whole numbers.
                    population = atoi(fieldBuffer);
                }

                fieldIndex++;
            }
        }

        /* Fold this city into the running answers */

        totalPopulation = totalPopulation + population;

        /* Northernmost means largest latitude, since latitude increases
        going north.
        
        The citiesRead == 0 test handles the first city: 
        there is nothing to compare against yet, so it becomes the leader by
        default. 
        
        Starting highestLatitude at 0.0 instead would happen
        to work for US cities, but would silently break on anywhere
        south of the equator. */
        if (citiesRead == 0 || latitude > highestLatitude) {
            highestLatitude = latitude;
            strcpy(northernmostCity, cityName);
        }

        citiesRead++;
    }

    fclose(inFile);

    // Reports
    if (citiesRead == 0) {
        printf("No city data found in %s\n", filename);
        return 1;
    }

    printf("Sum of the populations: %d\n", totalPopulation);
    printf("Northernmost city is %s\n", northernmostCity);

    /* CHECKER:  
    printf("(read %d cities; highest latitude %f)\n",
           citiesRead, highestLatitude);
    */

    return 0;
}