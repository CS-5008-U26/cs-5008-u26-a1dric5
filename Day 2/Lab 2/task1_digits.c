// I've included per line comments for my own sake so I can 
// go back and study my scripts 

#include <stdio.h>  // Standard library for input/output functions like printf and scanf

int main() {
    int n, sum = 0;  // 'n' stores the user's number, 'sum' starts at 0 and accumulates the digit total

    printf("Enter a number: ");
    scanf("%d", &n);  // Read an integer from the user and store it in 'n'

    // Keep looping until we've processed every digit
    while (n != 0) {
        sum += n % 10;  // % 10 gives us the rightmost digit, add it to sum
        n /= 10;        // / 10 removes the rightmost digit, move to next digit
    }

    printf("The sum of the digits is %d\n", sum);
    return 0;  // 0 means the program finished successfully
}