/* Accepts an integer n, then prints the ET-function for each 
integer from 2 to n. 

The ET-function et(x) is the number of integers less than x that are 
relatively prime with x. Two numbers are relatively prime if they have 
no factors in common other than 1. 
Ex: et(9) is 6 because 1, 2, 4, 5, 7, and 8 share no factor with 9 other 
than 1, while 3 and 6 both share the factor 3. 
*/

#include <stdio.h>

/* hasCommonFactor tells whether two positive integers share a factor 
other than 1 

params: 
int a, the first number 
int b, the second number 

returns: 
int(bool), 1 if a and b share a factor greater than 1. 0 if they don't, meaning 
they are relatively prime 

if (!hasCommonFactor (k, x)) { // k and x are relatively prime}
*/

int hasCommonFactor (int a, int b) {
    int d; 

    /* start at 2 because 1 divides everything so it would tell us nothing. 
    A shared factor can never be bigger than the smaller of the two numbers, 
    so stop once d passes either one 
    */
    for (d = 2; (d <= a) && (d <= b); d++) {
        if (((a % d) == 0) && ((b % d) == 0)) {
            return (1); 
        }
    }
    return (0);     // nothing shared, they are relatively prime 
}

/* et, counts how many integers below x are relatively prime with x 

params: 
int x, the number being examined 

returns: 
int, how many integers k, with 1 <= x, share not factor with x other than 1 

printf ("%d", et (9));  // prints 6 

this tests every candidate one at a time, so it gets slow for large x. 
*/
int et (int x) {
    int count = 0;  // how many prime numbers so far 
    int k;          // the candidate being tested 


    /* The definition says integer less than x, so x itself is not 
    counted. k = 1 always counts, because the loop inside hasCommonFactor 
    never runs when a is 1 
    */
    for (k = 1; k < x; k++) {
        if (!hasCommonFactor (k, x)) {
            count = count + 1; 
        }
    }
    return (count); 
}

int main() {
    char s[100]; 
    int n; 
    int x; 

    printf("Enter an integer: "); 
    scanf("%d", &n);
    fgets(s, 100, stdin); 

    if (n < 2) {
        printf("Must be 2 or more\n");
    } else {
        for (x = 2; x <= n; x++) {
            printf("et(%d) = %d\n", x, et (x)); 
        }
    }   

    fgets(s, 100, stdin); 
}