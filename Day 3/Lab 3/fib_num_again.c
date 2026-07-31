#include <stdio.h>
#include <time.h>

#define RUNSEDONDS 5
#define SHOWALL 1 
#define BIGGESTINT 2147483647

int main() {
    char s[100]; 
    time_t start; 
    int n; 
    int previous;       // F(n-1) 
    int beforeThat;     // F(n-2) 
    int f;              // F(n), the one being computed 
    int ranOutOfRoom;   // 1 if we stopped because of in limits 

    start = time (NULL); 
    ranOutOfRoom = 0; 

    printf("Computing Fibonacci numbers iteratively for %d seconds ...\n", 
            RUNSEDONDS); 

    /* F(0) and F(1) are both 1 and are given, not computed.
    */
    beforeThat = 1;     // stands for F(0)
    previous = 1;       // stands for F(1) 
    f = 1; 
    n = 1; 

#ifdef SHOWALL 
    printf("F(0) = 1\n");
    printf("F(1) = 1\n");
#endif

    while ((time (NULL) - start) < RUNSEDONDS) {
       if (previous > (BIGGESTINT - beforeThat)) {
            ranOutOfRoom = 1; 
            break; 
       }

       f = previous + beforeThat;    // the next Fib num
       n = n + 1; 

#ifdef SHOWALL 
       printf("F(%d) = %d\n", n, f);
#endif

       /* slide the two tracking vars forward one step. The order matters 
       because beforeThat must take the old value or previous, so it has to 
       be assigned first 
       */
       beforeThat = previous; 
       previous = f; 
    }

    printf ("Highest reached: F(%d) = %d\n", n, f);

    if (ranOutOfRoom) {
        printf("Stopped because the next number would not fit in an int,\n");
        printf("not because time ran out.\n");
    }

    fgets(s, 100, stdin); 
}