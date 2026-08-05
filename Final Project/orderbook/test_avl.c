/* This is the test harness for the AVL tree module

The proposal promised: "Test harness inserts random and sorted
sequences and asserts balance and ordering invariants after every
operation."  This program does exactly that, in four tests:

Test 1: insert 1..N in SORTED order (the plain-BST killer input),
        checking invariants after every insert.
Test 2: insert N shuffled keys, checking after every insert.
Test 3: delete every key from the sorted-input tree one at a time,
        checking after every delete.
Test 4: height report, shows the tree height staying near
        log2(N) even on sorted input, which is the whole thesis
        of the project.

Exit status is 0 if every check passed, 1 if anything failed
(the class convention for main's return value).

Build and run: make test */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "book.h"

#define TEST_N 1000     // how many keys each test uses

/* checkOrDie		        runs the invariant checker and reports a failure
Parameters:
struct PriceLevel *root		the tree to check
char *what			        description printed on failure

Returns:
int	0 if the tree is valid, -1 if not */
int checkOrDie(struct PriceLevel *root, char *what) {
    if (levelCheckInvariants(root, LONG_MIN, LONG_MAX) < 0) {
        printf("FAILED: %s\n", what);
        return (-1);
    }
    return (0);
}

/* main			runs all the tests

Returns:
int	            0 if all tests pass, 1 if any test fails */
int main(void) {
    struct PriceLevel *root;
    struct PriceLevel *found;
    long i;

    // Test 1: sorted insertion, invariants checked after every step
    printf("Test 1: insert 1..%d in sorted order... ", TEST_N);
    root = NULL;
    for (i = 1; i <= TEST_N; i++) {
        root = levelInsert(root, i, &found);
        if ((found == NULL) || (found->price != i)) {
            printf("FAILED: insert did not report the new level\n");
            exit(1);
        }
        if (checkOrDie(root, "invariants after sorted insert") != 0) {
            exit(1);
        }
    }
    printf("passed\n");

    // keep this tree for Tests 3 and 4; build a second one for Test 2

    // Test 2: shuffled insertion, invariants checked after every step
    printf("Test 2: insert %d shuffled keys... ", TEST_N);

    /* build the keys 1..N in an array, then shuffle by swapping each
    slot with a random other slot (arrays + rand from class) */
    long keys[TEST_N];
    for (i = 0; i < TEST_N; i++) {
        keys[i] = i + 1;
    }
    srand(5008); // fixed seed so the test is reproducible
    for (i = 0; i < TEST_N; i++) {
        long j = rand() % TEST_N;
        long tmp = keys[i];
        keys[i] = keys[j];
        keys[j] = tmp;
    }

    struct PriceLevel *root2 = NULL;
    for (i = 0; i < TEST_N; i++) {
        root2 = levelInsert(root2, keys[i], &found);
        if (checkOrDie(root2, "invariants after shuffled insert") != 0) {
            exit(1);
        }
    }
    // also verify every key is findable
    for (i = 1; i <= TEST_N; i++) {
        if (levelFind(root2, i) == NULL) {
            printf("FAILED: key %ld missing after shuffled inserts\n", i);
            exit(1);
        }
    }
    printf("passed\n");

    /* Test 4 (before deleting anything): the height report
    log2(1000) is about 10; AVL guarantees height < 1.44 * log2(n),
    so anything under ~15 proves balance held.  A plain BST fed the
    sorted input of Test 1 would have height 1000 here. */
    printf("Height report for %d keys (log2 is about 10):\n", TEST_N);
    printf("  sorted-input tree height:   %d\n", levelHeight(root));
    printf("  shuffled-input tree height: %d\n", levelHeight(root2));
    if (levelHeight(root) > 15) {
        printf("FAILED: sorted-input tree is too tall -- not balancing\n");
        exit(1);
    }


    // Test 3: delete every key one at a time, checking every step
    printf("Test 3: delete all %d keys one by one... ", TEST_N);
    /* delete odd keys first, then even, so we exercise deletes at the
    leaves, in the middle, and at the root in many different shapes */
    for (i = 1; i <= TEST_N; i = i + 2) {
        root = levelDelete(root, i);
        if (checkOrDie(root, "invariants after delete (odd pass)") != 0) {
            exit(1);
        }
    }
    for (i = 2; i <= TEST_N; i = i + 2) {
        root = levelDelete(root, i);
        if (checkOrDie(root, "invariants after delete (even pass)") != 0) {
            exit(1);
        }
    }
    if (root != NULL) {
        printf("FAILED: tree not empty after deleting every key\n");
        exit(1);
    }
    printf("passed\n");

    // clean up the shuffled tree (ASan verifies nothing leaks)
    struct Book cleanup;
    bookInit(&cleanup);
    cleanup.bids = root2;
    bookFree(&cleanup);

    printf("All AVL tests passed.\n");
    return (0);
}
