#include <stdio.h>

int main() {
    int score;  // Stores the user's number score

    printf("Enter the number score: ");
    scanf("%d", &score);  // Read an integer from the user

    // Check for invalid input first (less than 1 or greater than 10)
    if (score < 1 || score > 10) {
        printf("Error: Please enter a number between 1 and 10.\n");

    } else if (score <= 3) {    // 1, 2, or 3 is F
        printf("Your grade is F\n");

    } else if (score <= 5) {    // 4 or 5 is E
        printf("Your grade is E\n");

    } else if (score == 6) {    // 6 is D
        printf("Your grade is D\n");

    } else if (score == 7) {    // 7 is C
        printf("Your grade is C\n");

    } else if (score == 8) {    // 8 is B
        printf("Your grade is B\n");

    } else {                    // 9 or 10 is A
        printf("Your grade is A\n");
    }

    return 0;
}