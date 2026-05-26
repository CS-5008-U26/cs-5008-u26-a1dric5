// I've included per line comments for my own sake so I can 
// go back and study my scripts 

#include <stdio.h>

int main() {
    float price;  // Stores the price the user enters (float allows decimal values)

    // Loop forever until the user enters 0
    while (1) {
        printf("Enter a price in dollars: ");
        scanf("%f", &price);  // %f reads a floating-point number into 'price'

        // If user enters 0, exit the loop immediately
        if (price == 0) break;

        float total = price * 1.08;  // Multiply by 1.08 to add 8% tax
                                     

        // %.2f prints the result rounded to exactly 2 decimal places (pennies)
        printf("The value with tax is $%.2f\n", total);
    }

    // Program exits cleanly when user enters 0, printing nothing more
    return 0;
}