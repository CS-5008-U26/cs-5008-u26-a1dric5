/*
* This file asks the user for an integer and prints it's 10's 
complement, where the 10's complement replaces each digit d with (9 - d).
For example: 123 would be 876. 
This also uses a recursive function. 
*/

#include <stdio.h>

/* complement, computes the 10's complement of a non-negative integer 
paramenters: 
int x, the number to complement (must be >=0) 

returns: 
int, x with every digit d replaced by (9 - d)

usage: 
int c = complement(123);  // c is 876 

Because the result is an int, the leading zeroes would disappear: 
complement (923) is 076, but it will print as 76. 
*/
int complement (int x) {
    int rightDigit;         // the rightmost digit of x 
    int restOfx;            // x with the rightmost digit removed 

    /* This is the base case. 
    A single digit has nothing to the left of it, so there is no 
    recursive call to make. 
    This is what stops the recursion. 
    */
    if (x < 10) {
        return (9 - x);
    }

    /* Recursive case: split x apart, complement the left part by calling 
    ourseleces on a SMALLER number, then rebuild 
    */
    int rightDigit = x % 10;    // strip off the rightmost digit 
    int restOfX = x / 10;       // everything to the left of it 


    return ((complement (restOfX) * 10) + (9 - rightDigit));
}

int main () {

    char s[100];
    int x; 

    printf("Enter an integer: ");
    scanf("%d", &x);
    fgets (s, 100, stdin);

    if (x < 0) {
        printf("Must be 0 or more\n");
    }    else {
        printf("The complement is %d\n", complement(x));
    }

    fgets(s, 100, stdin);
}