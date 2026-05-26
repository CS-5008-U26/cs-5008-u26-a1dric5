// I've included per line comments for my own sake so I can 
// go back and study my scripts 

#include <stdio.h>

int main() {
    // Loop from 0 to 1500, checking every number
    for (int i = 0; i <= 1500; i++) {

        // % 63 checks if i divides evenly by 63 (no remainder = it's a multiple)
        if (i % 63 == 0) {

            // %6d right-justifies the number in a 6-character wide column
            // This makes all numbers line up neatly on the right side
            printf("%6d\n", i);
        }
    }
    return 0;
}