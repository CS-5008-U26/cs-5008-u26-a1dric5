#include <stdio.h>  

int main() {
    float rate, hours, pay;  // rate = hourly rate, hours = hours worked, pay = total owed

    // Loop forever until the user enters 0 or a negative number
    while (1) {

        // Ask for hourly rate
        printf("Enter an hourly rate: ");
        scanf("%f", &rate);

        // If rate is 0 or less, exit without asking for hours
        if (rate <= 0) break;

        // Ask for number of hours
        printf("Enter the number of hours worked: ");
        scanf("%f", &hours);

        // If hours is 0 or less, exit without printing pay
        if (hours <= 0) break;

        // Calculate pay based on whether hours exceed 40
        if (hours <= 40) {
            // Simple case: pay is just rate x hours
            pay = rate * hours;
        } else {
            // Overtime case: first 40 hours at normal rate,
            // anything over 40 is paid at 1.5x the rate
            pay = (40 * rate) + ((hours - 40) * rate * 1.5);
        }

        printf("You should be paid %.4g\n", pay);  // %.4g prints up to 4 significant digits 
    }

    return 0;  // Program exits
}