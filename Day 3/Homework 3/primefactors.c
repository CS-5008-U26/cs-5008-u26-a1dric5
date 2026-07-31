#include <stdio.h>

int main () {

    char s[100];

    int n;
    int divisor;
    int isFirst;       // 1 until the first factor is printed 

    printf("Enter a number: ");
    scanf("%d", &n);
    fgets(s, 100, stdin); 

    if (n < 2) {
        printf("Must be 2 or more\n");
    } else {
        printf("The prime factors are ");
        divisor = 2;                        // the smallest possible prime factor 
        isFirst = 1; 

        while(n > 1) {                      // keep going until n is worn down to 1 
            if ((n % divisor) ==0) {
                /* divisor is a factor of n, and it must be PRIME, because 
                every smaller factor has already been divided out of n 
                and so cannot be hiding inside divisor.
                */
                if(!isFirst) {
                    printf(" * ");  // seperators go between factors only
                }
                printf("%d", divisor);
                isFirst = 0;
                n = n / divisor;    // divide it out, but keep the same 
                                    // divisor because factors can repeat 

            } else {
                divisor = divisor + 1;       // try the next candidate 
            }
        }
        printf("\n");
    }

    fgets(s, 100, stdin);
}