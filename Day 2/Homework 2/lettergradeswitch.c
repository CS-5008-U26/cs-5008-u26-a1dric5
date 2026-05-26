#include <stdio.h>

int main() {
    int score;  // Stores the user's number score

    printf("Enter the number score: ");
    scanf("%d", &score);  // Read an integer from the user

    // switch checks the exact value of 'score' and jumps to the matching case
    switch (score) {
        case 1:             // Score of 1 falls through to case 2
        case 2:             // Score of 2 falls through to case 3
        case 3:             // All three land here and print F
            printf("Your grade is F\n");
            break;          // break exits the switch block

        case 4:             // Score of 4 falls through to case 5
        case 5:
            printf("Your grade is E\n");
            break;

        case 6:
            printf("Your grade is D\n");
            break;

        case 7:
            printf("Your grade is C\n");
            break;

        case 8:
            printf("Your grade is B\n");
            break;

        case 9:             // Score of 9 falls through to case 10
        case 10:
            printf("Your grade is A\n");
            break;

        default:            // Catches anything less than 1 or greater than 10
            printf("Error: Please enter a number between 1 and 10.\n");
            break;
    }

    return 0;
}