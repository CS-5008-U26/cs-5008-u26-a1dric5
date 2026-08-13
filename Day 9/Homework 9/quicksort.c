/* This file implements Quick Sort using the provided sort template.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TO_PRINT 100        // max number of elements to print
#define MAX_VALUE 100000000     // max value in randomly generated data
#define BUFFER_SIZE 100         // size of the input buffer

/* T E S T   C A S E   D A T A */

int test1[] =       { 3 };
int expected1[] =   { 3 };
int n1 =            1;
int test2[] =       { 5, 3 };
int expected2[] =   { 3, 5 };
int n2 =            2;
int test3[] =       { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int expected3[] =   { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int n3 =            10;
int test4[] =       { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
int expected4[] =   { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int n4 =            10;
int test5[] =       { 2, 4, 6, 8, 10, 1, 3, 5, 7, 9 };
int expected5[] =   { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int n5 =            10;
int test6[] =       { 1, 2, 3, 2, 2 };
int expected6[] =   { 1, 2, 2, 2, 3 };
int n6 =            5;

/* Y O U R   S O R T   F U N C T I O N */

/* partition        rearrange a slice around a pivot value

Parameters:
a                   the array being sorted
left                first index of the slice
right               last index of the slice

Returns:
the index where the pivot value ended up

This uses the first element of the slice as the pivot. Everything
smaller than the pivot is swept to the left side, everything else stays
on the right, and the pivot is finally swapped into the boundary
between them. At that point, the pivot is in its permanent position. */

int partition (int a[], int left, int right) {
    int pivot = a[left];
    int boundary = left;                    // last index known to be < pivot

    for (int j=left+1; j<=right; j++) {
        if (a[j] < pivot) {
            boundary++;
            int temp = a[boundary];
            a[boundary] = a[j];
            a[j] = temp;
        }
    }

    int temp = a[left];                     // drops the pivot into place
    a[left] = a[boundary];
    a[boundary] = temp;

    return (boundary);
}

/* quicksortrange       recursively quicksort one slice of the array

Parameters:
a                       the array being sorted
left                    first index of the slice
right                   last index of the slice

Returns:
nothing                 a[left...right] is left in ascending order*/

void quicksortrange (int a[], int left, int right) {
    while (left < right) {
        int p = partition (a, left, right);

        if ((p - left) < (right - p)) {     // left piece is smaller
            quicksortrange (a, left, p-1);
            left = p + 1;                   // loop on the right piece
        } else {                            // right piece is smaller
            quicksortrange (a, p+1, right);
            right = p - 1;                  // loop on the left piece
        }
    }
}

/* sortarray        sort an array of ints into ascending order. This is a quick sort.

Parameters:
a                   the array of ints to sort; it is modified in place
n                   the number of elements in the array

Returns:
nothing             the array a is left in ascending order


On random data the pivot lands somewhere near the middle most of the
time, each partition roughly halves the problem, and the sort runs in
O(n log n).

On sorted data it collapses. The first element is already the smallest
value in the slice, so partitioning puts zero elements on its left and
n-1 on its right. The problem shrinks by one element per partition
instead of halving, and the sort degrades to O(n^2), the same shape
as Bubble Sort, on the input a smarter sort finds easiest. Reverse
sorted data fails the same way for the same reason.

The standard fix is to choose a better pivot, median of three
(first, middle, last) or a random index, which makes the sorted case
as fast as the random case.
(I kept the naive pivot here so the degradation shows up in the timing table) */

void sortarray (int a[], int n) {

    quicksortrange (a, 0, n-1);
}

/* U T I L I T Y   F U N C T I O N S */

// printarray = print an array of ints
// a is the array, n is the number of elements

void printarray (int a[], int n) {
    int numberToPrint = ((n < MAX_TO_PRINT) ? n : MAX_TO_PRINT);
    for (int i=0; i < numberToPrint; i++) {
        printf ("%d ", a[i]);
    }
    printf ("\n");
}

// genarray - return an array of random ints on the heap
// numberofelements is how many elements in the array
// the array is returned
// don't forget to free the array when you are finished using it

int *genarray (int numberofelements) {
    int *result = malloc (numberofelements * sizeof(int));
    if (result == NULL) {
        printf ("malloc failed");
    } else {
        // fill the result array with random numbers between 0 and MAX_VALUE
        for (int i=0; i<numberofelements; i++) {
            // we have a problem
            // On Windows the rand() function only gives a 15-bit random number
            // This will be between 0 and 32767
            // So we will fix this with a crude trick
            // We will generate two random numbers, one for the high-order bits and the other
            // for the low-order 14 bits
            long long int r1 = rand();
            long long int r2 = rand();
            long long int randomValue = (r1 << 14) + (r2 & 0x3fff);
            result[i] = (int)(randomValue % MAX_VALUE);
        }
    }   
    return (result);
}

/* gensortedarray       return an array of ints on the heap, already in order

Parameters:
numberofelements        how many elements the array should have

Returns:
a pointer to the new array, or NULL if the malloc failed


The array is filled so that a[0] = 0, a[1] = 1, ... a[n-1] = n-1.
This is the best case for Bubble Sort and Insertion Sort, and the
worst case for a naive QuickSort that always picks the first element
as its pivot.

I need to free the array when I'm finished using it!*/

int *gensortedarray (int numberofelements) {
    int *result = malloc (numberofelements * sizeof(int));
    if (result == NULL) {
        printf ("malloc failed");
    } else {
        for (int i=0; i<numberofelements; i++) {
            result[i] = i;
        }
    }
    return (result);
}

/* genreversearray      return an array of ints on the heap, in reverse order

Parameters:
numberofelements        how many elements the array should have

Returns:
a pointer to the new array, or NULL if the malloc failed

The array is filled so that a[0] = n-1, a[1] = n-2, ... a[n-1] = 0.
This is the worst case for Bubble Sort and Insertion Sort, because every
adjacent pair starts out in the wrong order and nothing can be skipped.

I need to free the array when I'm finished using it!*/

int *genreversearray (int numberofelements) {
    int *result = malloc (numberofelements * sizeof(int));
    if (result == NULL) {
        printf ("malloc failed");
    } else {
        for (int i=0; i<numberofelements; i++) {
            result[i] = numberofelements - 1 - i;
        }
    }
    return (result);
}

// timedsort - runs a sort and records the elapsed time
// a is the array, n is how many elements

void timedsort (int a[], int n) {
    clock_t startTime = clock();        // get the start time
    sortarray (a, n);
    clock_t endTime = clock();          // get the end time
    double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf ("Result: ");                // print the sorted data
    printarray (a, n);
    printf ("Elapsed time: %f seconds\n\n", elapsedTime);   // print the elapsed time
}

// testsort - runs a sort on a test case with known results
// a is the array, n is how many elements
// expected is the array with the expected correct values after sorting

void testsort (int a[], int n, int expected[]) {
    printf ("Test case: ");             // print the test data
    printarray (a, n);
    sortarray (a, n);
    printf ("Result: ");                // print the result of sorting
    printarray (a, n);
    int ok = 1;                         // print whether the result is correct
    for (int i=0; ok && (i<n); i++) {
        ok = (a[i] == expected[i]);
    }
    printf ("%s\n\n", (ok ? "PASSED" : "FAILED"));
}

/* M A I N   F U N C T I O N */

int main () {
    char buffer[BUFFER_SIZE];
    int nelements;
    int *data;
    int keepgoing;

    srand (time(NULL));  // seed the random number generator

    // run test cases
    testsort (test1, n1, expected1);
    testsort (test2, n2, expected2);
    testsort (test3, n3, expected3);
    testsort (test4, n4, expected4);
    testsort (test5, n5, expected5);
    testsort (test6, n6, expected6);

    // run timed sorts on RANDOM data
    printf ("=== RANDOM DATA ===\n");
    keepgoing = 1;
    while (keepgoing) {
        printf ("How many elements? ");
        fgets (buffer, BUFFER_SIZE, stdin);
        if (buffer[0] == '\n') {
            keepgoing = 0;
        } else {
            nelements = atoi (buffer);
            if (nelements <= 0) {
                printf ("Must be a positive number of elements\n");
            } else {
                data = genarray (nelements);
                if (data != NULL) {
                    timedsort (data, nelements);
                    free (data);
                }
            }
        }
    }

    // run timed sorts on SORTED data
    printf ("=== SORTED DATA ===\n");
    keepgoing = 1;
    while (keepgoing) {
        printf ("How many elements? ");
        fgets (buffer, BUFFER_SIZE, stdin);
        if (buffer[0] == '\n') {
            keepgoing = 0;
        } else {
            nelements = atoi (buffer);
            if (nelements <= 0) {
                printf ("Must be a positive number of elements\n");
            } else {
                data = gensortedarray (nelements);
                if (data != NULL) {
                    timedsort (data, nelements);
                    free (data);
                }
            }
        }
    }

    // run timed sorts on REVERSE SORTED data
    printf ("=== REVERSE SORTED DATA ===\n");
    keepgoing = 1;
    while (keepgoing) {
        printf ("How many elements? ");
        fgets (buffer, BUFFER_SIZE, stdin);
        if (buffer[0] == '\n') {
            keepgoing = 0;
        } else {
            nelements = atoi (buffer);
            if (nelements <= 0) {
                printf ("Must be a positive number of elements\n");
            } else {
                data = genreversearray (nelements);
                if (data != NULL) {
                    timedsort (data, nelements);
                    free (data);
                }
            }
        }
    }

    return (0);
}