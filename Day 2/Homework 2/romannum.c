#include <stdio.h>

int main() {
    int number;     // Stores the user's input

    printf("Enter a number between 1 and 4000: ");
    scanf("%d", &number);

    // Validates input
    if (number < 1 || number > 4000) {
        printf("Error: Please enter a number between 1 and 4000.\n");
        return 0;
    }

    // Roman numeral conversion
    // The key insight: list all values (including subtractive combos like
    // IV, IX, XL, etc.) from largest to smallest.
    // Repeatedly subtract the largest value that fits, printing its
    // symbol each time, until nothing remains.

    // REFERENCE RESOURCE: 
    // Modern C for Absolute Beginners, 2nd ed., by S Dmitrovic, Apress 2024.
    // Parallel arrays: values[i] matches symbols[i]
    int values[]       = { 1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1 };
    char * symbols[]    = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };

    int numSymbols = 13;  // Total number of entries in our arrays

    printf("The Roman Numeral: ");

    // Loop through each symbol from largest to smallest
    for (int i = 0; i < numSymbols; i++) {

        // Keep subtracting this value as long as it still fits in 'number'
        while (number >= values[i]) {
            printf("%s", symbols[i]);   // Print the matching Roman symbol
            number -= values[i];        // Subtract the value from remaining number
        }
    }

    printf("\n");

    return 0;
}