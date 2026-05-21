#include <stdio.h>
int main() {
    char s[100];    // s stores the user's name
    char t[100];    // t is used to clear stdin and pause the console
    /* Ask for the user's name */
    printf ("What is your name? ");
    scanf("%s", s);
    fgets(s,100,stdin);     // clear the leftover /n from scanf (store in t, not s)
    
    /* Print response using the name */
    printf("%s? That's a funny name!",t);   // use s (the name), not t 
    
    /* Pause the console so it doesn't close immediately */
    fgets(t,100,stdin);
}
