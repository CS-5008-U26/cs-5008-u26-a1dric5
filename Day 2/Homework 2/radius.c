#include <stdio.h>
#include <math.h>   // Needed for acos() to calculate pi

int main() {
    int radius;     // Stores the user's input radius

    // Calculate pi using the math library's acos function
    // acos(-1) = pi because the arc cosine of -1 is exactly pi
    double pi = acos(-1);

    printf("Enter an integer radius: ");
    scanf("%d", &radius);   // Read an integer from the user

    // Calculate area = pi * radius^2
    // Cast radius to double so the multiplication uses decimal precision
    double area = pi * (double)radius * (double)radius;

    // (int) truncates the decimal, giving us an integer approx.
    printf("For a circle of radius %d the area is %d\n", radius, (int)area);

    return 0;
}