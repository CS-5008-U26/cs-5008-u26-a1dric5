#include <stdio.h>

/* this prints the prime factorization of n, smallest factor 
first, with " * " between successive factors. 

params: 
int n, the number remaining to be factored 
int divisor, the smallest candidate factor still worth trying;
            the first call should pass 2 

returns: 
void, this function exists for its side effect, printing 

ex: 
printFactors (180, 2);  --> prints 2 * 2 * 3 * 3 * 5
*/
void printFactors (int n, int divisor) {   
    /* this is the base case: n has been worn down to 1, so every factor has already 
    been printed and there is nothing left to do 
    */ 
    if (n <= 1) {
        return; 
    }

    /* This divisor is not a factor, so move up to the next candidate.  
    Nothing is printed here, continue the search 
    */
    if ((n % divisor) != 0) {
        printFactors (n, divisor + 1); 
    }

    /* divisor is a factor of n, and it must be prime, because every smaller 
    factor has already been divided out of n 
    */
    printf ("%d", divisor); 

    /* n / divisor is what will be left after this factor is divided out. 
    If that is still more than 1, another factor is coming and a separator belongs 
    here. If it is exactly 1, this was the last factor and no need to print a trailing 
    separator. 
    */
    if ((n / divisor) > 1) {
        printf (" * "); 
    }

    /* Recurses on the smaller number, keeping the same divisor because a factor 
    can repeate. Ex: 180 / 2 is 90, which is divisible by 2 again 
    */
    printFactors(n / divisor, divisor);
}

int main() {
    char s[100]; 
    int n; 

    printf("Enter a number: ");
    scanf("%d", &n); 
    fgets (s, 100, stdin); 

    if (n < 2) {
        printf ("Must be 2 or more \n"); 
    } else {
        printf ("The prime factors are "); 
        printFactors (n, 2);                // 2 is the smallest possible prime factor 
        printf ("\n"); 
    }

    fgets (s, 100, stdin);
}