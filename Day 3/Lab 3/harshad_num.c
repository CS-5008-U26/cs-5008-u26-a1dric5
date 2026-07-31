#include <stdio.h>

int isHarshad (int x) {
    int digitSum = 0; 
    int rest = x; 

    while (rest > 0) { 
        digitSum = digitSum + (rest % 10);
        rest = rest / 10; 
    }

    return ((x % digitSum) == 0); 
}

int main() {
    char s[100]; 
    int x; 

    printf("The 2 digit Harshad numbers are:\n"); 

    for (x = 10; x <= 99; x++) {
        if (isHarshad (x)) {
            printf("%d\n", x); 
        }
    }

    fgets (s, 100, stdin);
}