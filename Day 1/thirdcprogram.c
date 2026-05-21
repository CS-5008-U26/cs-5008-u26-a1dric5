#include <stdio.h>
int main() {
    char s[100];    // s stores the user's name
    char t[100];    // t is used to clear stdin and pause the console
    int age;        // age stores the user's age 

    /* Ask for the user's name */
    printf ("What is your name? ");
    scanf("%s", s);
    fgets(t,100,stdin);     // clear the leftover /n from scanf (store in t, not s)
    
    /* Print response using the name */
    printf("%s? That's a funny name!\n",s);   // use s (the name), not t 

    /* Ask for the user's age */
    printf("How old are you, %s? ", s);
    scanf("%d", &age);
    fgets(t,100,stdin);     // clear the leftover /n from scanf

    /* Print response using the age */
    printf("Wow, %d years old! Gettin' older!\n", age);
    
    /* Pause the console so it doesn't close immediately */
    fgets(t,100,stdin);
}
