// I've included per line comments for my own sake so I can 
// go back and study my scripts 

#include <stdio.h>
#include <time.h>   // Needed for clock() to measure how long the program takes

int main() {
    clock_t start = clock();  // Record the time before we start finding primes

    // Check every number from 2 to 100 (1 is not prime by definition)
    for (int n = 2; n <= 100; n++) {
        int isPrime = 1;  // Assume the number is prime until proven otherwise

        // Try dividing 'n' by every integer from 2 up to n-1
        for (int d = 2; d < n; d++) {
            if (n % d == 0) {    // If it divides evenly, it's not prime
                isPrime = 0;     // Mark it as not prime
                break;           // No need to keep checking, exit inner loop early
            }
        }

        if (isPrime) printf("%d\n", n);  // Only print if still marked as prime
    }

    clock_t end = clock();  // Record the time after we're done

    // Subtract start from end and convert to seconds to get elapsed time
    printf("Time: %.6f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}