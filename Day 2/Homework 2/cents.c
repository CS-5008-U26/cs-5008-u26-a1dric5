#include <stdio.h>

int main() {
    int cents;      // Stores the user's input
    int quarters, dimes, nickels, pennies;  // Stores the count of each coin

    printf("Enter a number of cents: ");
    scanf("%d", &cents);

    // Validates input
    if (cents < 1 || cents > 100) {
        printf("Error: Please enter a number between 1 and 100.\n");
        return 0;
    }

    // Calculates coins using largest denominations first 
    quarters = cents / 25;      // How many 25-cent coins fit?
    cents     = cents % 25;     // Remaining cents after quarters

    dimes    = cents / 10;      // How many 10-cent coins fit in the remainder?
    cents    = cents % 10;      // Remaining cents after dimes

    nickels  = cents / 5;       // How many 5-cent coins fit in the remainder?
    cents    = cents % 5;       // Remaining cents after nickels

    pennies  = cents;           // Whatever is left are just pennies

    // Builds the output string, omitting coins with 0 count
    // Tracks how many coin types have been printed to handle
    // commas and "and" correctly
    int total_coins = (quarters > 0) + (dimes > 0) + (nickels > 0) + (pennies > 0);
    int printed = 0;  // Counts how many coin types we've printed so far

    printf("That requires ");

    // Prints each coin type if count > 0
    // For each coin: use singular if count == 1, plural otherwise
    // Add "and" before the last coin, comma after non-last coins

    if (quarters > 0) {
        printed++;
        // Check if this is the last coin to add "and" before it
        if (printed == total_coins && total_coins > 1) printf("and ");
        printf("%d %s", quarters, quarters == 1 ? "quarter" : "quarters");
        // Add comma if more coins follow
        if (printed < total_coins) printf(", ");
    }

    if (dimes > 0) {
        printed++;
        if (printed == total_coins && total_coins > 1) printf("and ");
        printf("%d %s", dimes, dimes == 1 ? "dime" : "dimes");
        if (printed < total_coins) printf(", ");
    }

    if (nickels > 0) {
        printed++;
        if (printed == total_coins && total_coins > 1) printf("and ");
        printf("%d %s", nickels, nickels == 1 ? "nickel" : "nickels");
        if (printed < total_coins) printf(", ");
    }

    if (pennies > 0) {
        printed++;
        if (printed == total_coins && total_coins > 1) printf("and ");
        printf("%d %s", pennies, pennies == 1 ? "penny" : "pennies");
    }

    printf(".\n");

    return 0;
}