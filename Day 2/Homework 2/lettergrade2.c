#include <stdio.h>

int main() {
    int score;          // Stores the user's score (0 to 100)
    char grade;         // Stores the letter grade (A, B, C, D, F)
    char modifier;      // Stores the modifier: '+', '-', or ' ' (space = nothing)

    printf("Enter the number score: ");
    scanf("%d", &score);

    //Validates input
    if (score < 0 || score > 100) {
        printf("Error: Please enter a number between 0 and 100.\n");
        return 0;  // Exit the program early if input is invalid
    }

    // Determines letter grade
    if (score <= 60) {
        grade = 'F';
    } else if (score <= 70) {
        grade = 'D';
    } else if (score <= 80) {
        grade = 'C';
    } else if (score <= 90) {
        grade = 'B';
    } else {
        grade = 'A';
    }

    // Determines modifier (+, -, or nothing) 
    // Only applies for scores above 60; F has no modifier
    if (score <= 60) {
        modifier = ' ';  // No modifier for F

    } else {
        int lastDigit = score % 10;  // % 10 gives us the last digit 

        if (lastDigit == 1 || lastDigit == 2 || lastDigit == 3) {
            modifier = '-';

        } else if (lastDigit == 8 || lastDigit == 9 || lastDigit == 0) {
            modifier = '+';

        } else {
            // lastDigit is 4, 5, 6, or 7 — no modifier
            modifier = ' ';
        }
    }

    // Prints the result
    // If modifier is a space, print nothing after the grade
    if (modifier == ' ') {
        printf("Your grade is %c\n", grade);
    } else {
        printf("Your grade is %c%c\n", grade, modifier);
    }

    return 0;
}