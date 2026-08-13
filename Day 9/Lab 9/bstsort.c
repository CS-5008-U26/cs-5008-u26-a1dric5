/* This file implements BST Sort using the provided sort template.
The array is loaded into a BBST, then an in order traversal dumps 
the values back into the array in sorted order.

Task 2 adds freeBst() to release the nodes when I'm done.
I set FREE_THE_TREE to 0 to measure the sort without freeing,
or to 1 to measure it with freeing. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TO_PRINT 100        // max number of elements to print
#define MAX_VALUE 100000000     // max value in randomly generated data
#define BUFFER_SIZE 100         // size of the input buffer
#define FREE_THE_TREE 1         // 1 frees the BST nodes, 0 leaks them

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

/* B S T   D A T A   S T R U C T U R E */

/*struct treenode  one node of the Binary Search Tree

Fields:
value               the integer stored at this node
left                subtree holding values LESS than value
right               subtree holding values GREATER THAN OR EQUAL TO value

Duplicates go to the right. Test case 6 contains duplicate values,
so the tree has to store them rather than discard them.*/

struct treenode {
    int value;
    struct treenode *left;
    struct treenode *right;
};

/* insertNode       insert one value into the BST

Parameters:
root                the current root of the tree, or NULL for an empty tree
value               the integer to insert

Returns:
the root of the tree after the insertion, or NULL if a malloc failed

This traverse down the tree iteratively rather than recursively.
A recursive insert would use one stack frame per level, and sorted input
builds a tree of depth n, that would overflow the stack long before reaching 
an interesting value of N. */

struct treenode *insertNode (struct treenode *root, int value) {
    struct treenode *newnode = malloc (sizeof(struct treenode));
    if (newnode == NULL) {
        printf ("malloc failed");
        return (root);
    }
    newnode->value = value;
    newnode->left = NULL;
    newnode->right = NULL;

    if (root == NULL) {
        return (newnode);               // the new node becomes the root
    }

    struct treenode *current = root;
    int placed = 0;
    while (!placed) {
        if (value < current->value) {
            if (current->left == NULL) {
                current->left = newnode;
                placed = 1;
            } else {
                current = current->left;
            }
        } else {
            if (current->right == NULL) {
                current->right = newnode;
                placed = 1;
            } else {
                current = current->right;
            }
        }
    }
    return (root);
}

/* dumpTree     copy the tree's values back into an array, in sorted order

Parameters:
node            the subtree to dump
a               the array to write into
index           pointer to the next free slot in a; updated through traversal

Returns:
nothing         values are written into a and *index is advanced


This is an in-order traversal: left subtree, then this node, then the
right subtree. In-order traversal of a BST always visits the values in
ascending order, which is the whole reason BST Sort works. 
index is passed by pointer so every recursive call shares one counter. */

void dumpTree (struct treenode *node, int a[], int *index) {
    if (node != NULL) {
        dumpTree (node->left, a, index);
        a[*index] = node->value;
        *index = *index + 1;
        dumpTree (node->right, a, index);
    }
}

/* freeBst      free every node in the tree

Parameters:
node            the subtree to free

Returns:
nothing         all the nodes are released back to the heap
 
This is a post-order traversal: both children are freed before the node
itself. The order matters. If the first node is freed, the pointers to
its children would be gone and the children would become unreachable; 
and reading node->left after free(node) is a use after free. */

void freeBst (struct treenode *node) {
    if (node != NULL) {
        freeBst (node->left);
        freeBst (node->right);
        free (node);
    }
}

/* Y O U R   S O R T   F U N C T I O N */

/* sortarray        sort an array of ints into ascending order. This is a BST SORT.

Parameters:
a                   the array of ints to sort; it is modified in place
n                   the number of elements in the array

Returns:
nothing             the array a is left in ascending order

Two phases: 
(1) insert all n values into a BST.
(2) do an in-order traversal to write them back into the array, which produces them 
in ascending order.

On random data each insert costs about O(log n), so building the tree is
O(n log n) and the traversal is O(n). On data that is already sorted the
tree degenerates into a linked list and the sort becomes O(n^2). */

void sortarray (int a[], int n) {

    struct treenode *root = NULL;

    // phase 1, build the tree
    for (int i=0; i<n; i++) {
        root = insertNode (root, a[i]);
    }

    // phase 2, dump the tree back into the array, in order
    int index = 0;
    dumpTree (root, a, &index);

    // phase 3, release the nodes
    if (FREE_THE_TREE) {
        freeBst (root);
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

I need to free the array when I'm inished using it!*/

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

The array is filled so that a[0] = 0, a[1] = 1, ... a[n-1] = n-1.
This is the best case for Bubble Sort and Insertion Sort, and the
worst case for a naive QuickSort that always picks the first element
as its pivot.

I need to free the array when I'm inished using it!*/

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