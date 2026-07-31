#include <stdio.h>
#include <stdlib.h> // for rand() and srand() 
#include <time.h>   // for time(), used to seed the generator 

int count (int p, int q, int r) {
    int hits = 0;   // how many landed in 0 to r 
    int i;          // which random number we are on 
    int number;     // the random number just generated 


    if ( p < 1) {
        return (-1); 
    }
    if ((q < 0) || (q >= RAND_MAX)) {
        return (-1); 
    }
    if ((r < 0) || (r > q)) {
        return (-1); 
    }

    for (i = 0; i < p; i++) {
        number = rand () % (q + 1); 
        if (number <= r) { 
            hits = hits + 1; 
        }
    }

    return (hits); 
}

int main() {
    char s[100]; 

    int p; 
    int q; 
    int r; 
    int result; 

    srand (time (NULL)); 

    printf ("Testing the parameter checks:\n");
    printf ("  count (0, 100, 50)   = %d   (p too small)\n",
            count (0, 100, 50));
    printf ("  count (-5, 100, 50)  = %d   (p negative)\n",
            count (-5, 100, 50));
    printf ("  count (10, -1, 50)   = %d   (q negative)\n",
            count (10, -1, 50));
    printf ("  count (10, 100, -1)  = %d   (r negative)\n",
            count (10, 100, -1));
    printf ("  count (10, 100, 200) = %d   (r bigger than q)\n",
            count (10, 100, 200));
    printf ("  count (10, 50, 50)   = %d   (all valid, so all 10 count)\n",
            count (10, 50, 50));
    printf ("\n");

    while (1) {
        printf("how many numbers (0 to quit)? ");
        scanf("%d", &p);
        fgets (s, 100, stdin); 

        if (p ==0) {
            break; 
        }

        printf ("Largest number to generate? ");
        scanf ("%d", &q);
        fgets (s, 100, stdin);
 
        printf ("Largest number to count? ");
        scanf ("%d", &r);
        fgets (s, 100, stdin);
 
        result = count (p, q, r);
        if (result < 0) {
            printf ("Those values are not valid\n");
        } else {
            printf ("%d of %d numbers were in the range 0 to %d\n",
                    result, p, r);
        }
        printf ("\n");
    }

    fgets (s, 100, stdin); 
}

