#include <stdio.h>
#include <time.h>

#define RUNSECONDS 5    // how long to let the program run
#define SHOWALL 1       // comment out to print only the highest 

int fibonacci (int n) {
    if (n < 2) {
        return (1);
    }

    return (fibonacci (n -1) + fibonacci (n - 2)); 
}

int main() {
    char s[100]; 
    time_t start;   // the clock time when start 
    int n;          // which Fib number we're at 
    int f;          // the value of F(n)
    int highestN;   // the largest n when finished 
    int highestF;   // the value of F(highestN)

    /* time(NULL) is the current clock time in seconds, the same 
    call used to seed the rand. num. generator 
    */
    start = time(NULL);

    n = 0; 
    highestN = 0; 
    highestF = 1; 

    printf("Computing Fibonacci numbers recursivley for %d seconds ...\n", 
    RUNSECONDS); 

    /*
    Keep going until RUNSECONDS have passed.
    The check happens before each call, so the very last call is allowed to 
    finish even if it runs past the limit. Because each number costs roughly 
    twice the one before, that last call can take a while. */
    while ((time (NULL) - start) < RUNSECONDS) {
        f = fibonacci(n); 
        highestF = n; 
        highestF = f; 
#ifdef SHOWALL
        printf("F(%d) = %d\n", n, f);
#endif 
        n = n + 1; 
    }

    printf("Highest reached in %d seconds: F(%d) = %d\n", 
            RUNSECONDS, highestN, highestF); 

    fgets(s, 100, stdin); 
}