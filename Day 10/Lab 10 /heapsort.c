/* This file implements heap sort using the provided sort template.

main function runs the built-in test cases

then runs 3 interactive timing loops: 
random data, sorted data, and reverse sorted data.*/

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

/* heapifyDown      restore the heap property at one node by sifting it down

 Parameters:
a                   the array holding the heap; it is modified in place
n                   how many elements are currently IN THE HEAP (not the array size)
i                   the 1-based index of the node to sift down

Returns:
nothing             the subtree rooted at node i is a valid heap when this returns
 

The heap is 1 based: root is node 1, children of node i are 2i and 2i+1,
parent is i/2. The array is 0 based, so node i lives at a[i-1].

This is a max-heap, the opposite of the priority queue's min-heap, the
whole difference is the direction of the two comparisons. Heap sort 
parks each extracted value at the end, and that value is the largest.

The node sinks until both children are smaller, or it runs out of
children. Always swap with the larger child, swapping with the smaller
one leaves that child bigger than its new parent and breaks the heap.

Iterative rather than recursive: a sift-down is tail-recursive anyway,
so the loop is the same algorithm with no stack growth. */

void heapifyDown (int a[], int n, int i) {

    int keepgoing = 1;

    while (keepgoing) {
        int left = 2 * i;               // 1-based index of the left child
        int right = 2 * i + 1;          // 1-based index of the right child
        int largest = i;                // assume node i is already the largest

        if ((left <= n) && (a[left - 1] > a[largest - 1])) {
            largest = left;
        }
        if ((right <= n) && (a[right - 1] > a[largest - 1])) {
            largest = right;
        }

        if (largest == i) {
            keepgoing = 0;              // both children are smaller - done
        } else {
            int temp = a[i - 1];        // swap node i with its larger child
            a[i - 1] = a[largest - 1];
            a[largest - 1] = temp;
            i = largest;                // and keep sinking from there
        }
    }
}

/* sortarray    sort an array of ints into ascending order. This is a Heap Sort.
 
Parameters:
a               the array of ints to sort; it is modified in place
n               the number of elements in the array

Returns
nothing         the array a is left in ascending order
 

Two phases, both in place, no second array is ever allocated.

Phase 1: Build 
heapifyDown every node from n/2 down to 1. n/2 is the last node with a child; 
everything past it is a leaf, already a valid one element heap. Going downward 
guarantees both subtrees of node i are already heaps, which is what heapifyDown 
requires.

Phase 2: Extract
The root holds the largest value, which belongs in the last slot. Exchanges them, 
shrinks the heap by one so the parked value is never touched again, and heapifyDown 
the new root. Repeated until one element is left.

Phase 2 is n heapifyDowns at O(log n) each, so O(n log n). 
Phase 1 is O(n), not O(n log n), most nodes are near the bottom and barely sink.

HeapSort has no bad case: it cannot degrade to O(n^2) like Quick Sort, and
it has no early cutoff on sorted data like Bubble and Insertion Sort. */

void sortarray (int a[], int n) {

    if (n < 2) {
        return;                         // 0 or 1 elements is already sorted
    }

    // Phase 1: build a max-heap out of the whole array
    for (int i = n / 2; i >= 1; i--) {
        heapifyDown (a, n, i);
    }

    // Phase 2: repeatedly move the root to the end and shrink the heap
    for (int heapsize = n; heapsize > 1; heapsize--) {
        // exchange the root with the last element 
        int temp = a[0];                    
        a[0] = a[heapsize - 1];             
        a[heapsize - 1] = temp;
        heapifyDown (a, heapsize - 1, 1);   // repair the now-smaller heap
    }
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