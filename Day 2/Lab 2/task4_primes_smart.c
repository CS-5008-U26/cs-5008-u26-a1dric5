// I've included per line comments for my own sake so I can 
// go back and study my scripts 

#include <stdio.h>
#include <math.h>   // Needed for sqrt() function
#include <time.h>   // Needed for clock() to measure elapsed time

int main() {
    clock_t start = clock();  // Record start time

    for (int n = 2; n <= 100; n++) {

        // Special case: 2 is the only even prime, print it and skip to next number
        if (n == 2) { printf("%d\n", n); continue; }

        // Skip all other even numbers — they can't be prime
        if (n % 2 == 0) continue;

        int isPrime = 1;  // Assume prime until proven otherwise

        // Calculate square root ONCE per candidate number (more efficient)
        // Key insight: if n has a factor larger than sqrt(n),
        // it must also have one smaller, so we don't need to check beyond it
        int sqrtN = (int)sqrt((double)n);

        // Only check odd divisors (d += 2 skips even numbers)
        // and only up to the square root
        for (int d = 3; d <= sqrtN; d += 2) {
            if (n % d == 0) {   // Found a factor, not prime
                isPrime = 0;
                break;
            }
        }

        if (isPrime) printf("%d\n", n);
    }

    clock_t end = clock();  // Record end time
    printf("Time: %.6f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}