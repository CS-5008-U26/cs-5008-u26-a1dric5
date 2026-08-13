/* Priority Queue Using a Sorted List (Task 1)

A priority queue whose backing store is a dynamically sized array that is
kept in ascending sorted order.

Definitions to know (for myself!): 
ENQUEUE: append the new values to the end of the array, growing the 
backing store if necessary, then sort the whole array.

DEQUEUE: take the element at the front of the array (the lowest value), 
then copy every remaining element down one slot.

The program alternates between asking how many random numbers to add and
how many numbers to remove. Hitting ENTER at either prompt stops the
program. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 100             // size of the console input buffer
#define INITIAL_CAPACITY 16         // starting size of the backing store
#define MAX_VALUE 100000000         // max value in randomly generated data
#define MAX_TO_PRINT 100            // max number of values to actually print

/* The priority queue 

struct pqueue   a priority queue backed by a sorted array

Fields:
data            the backing store, a heap array kept in ascending order
count           how many elements are actually in the queue right now
capacity        how many elements the backing store can hold before it 
                has to be made bigger

The invariant is that data[0] ... data[count-1] are in ascending order,
so data[0] is always the highestm priority (lowest-valued) entry.
Slots from data[count] to data[capacity-1] are allocated but unused. */

struct pqueue {
    int *data;
    int count;
    int capacity;
};

/* pqInit       set up an empty priority queue

Parameters:
pq              pointer to the queue to initialize

Returns:
nothing

No memory is allocated here. The first enqueue allocates the backing
store, so a queue that is never used costs nothing. */

void pqInit (struct pqueue *pq) {
    pq->data = NULL;
    pq->count = 0;
    pq->capacity = 0;
}

/* pqFree       release the backing store of a priority queue

Parameters:
pq              pointer to the queue to free

Returns:
nothing

The queue is left in the same state as a freshly initialized one, so 
it is safe to call pqFree twice or to keep using the queue afterward. */

void pqFree (struct pqueue *pq) {
    free (pq->data);
    pq->data = NULL;
    pq->count = 0;
    pq->capacity = 0;
}

/* pqGrow       make sure the backing store can hold at least
                "needed" elements

Parameters:
pq              pointer to the queue
needed          the number of elements the backing store must be able to hold

Returns:
1 or 0          if the backing store is big enough // if the reallocation failed


The capacity is doubled each time it is too small, rather than grown by
a fixed amount. Doubling means that adding n elements one group at a
time costs O(n) copying in total instead of O(n^2), because each element
gets copied only a constant number of times on average.

If doubling still is not enough (someone asked for a huge group all at
once), the capacity jumps straight to what was asked for. */

int pqGrow (struct pqueue *pq, int needed) {

    if (needed <= pq->capacity) {
        return (1);                             // already big enough
    }

    int newCapacity = ((pq->capacity == 0) ? INITIAL_CAPACITY : pq->capacity);
    while (newCapacity < needed) {
        newCapacity = newCapacity * 2;
        if (newCapacity < 0) {                  // int overflow guard
            newCapacity = needed;
        }
    }

    int *newData = realloc (pq->data, newCapacity * sizeof(int));
    if (newData == NULL) {
        fprintf (stderr, "realloc failed - could not grow the queue to %d elements\n",
                 newCapacity);
        return (0);
    }

    pq->data = newData;
    pq->capacity = newCapacity;
    return (1);
}


/* Sorting the backing store 

merge       merge two adjacent sorted runs of an array back together

Parameters:
a           the array being sorted
lo          first index of the left run
mid         last index of the left run; the right run is mid+1 .. hi
hi          last index of the right run
scratch     a work array at least as large as a, used to hold the merge

Returns:
nothing     a[lo..hi] is left in ascending order*/

void merge (int a[], int lo, int mid, int hi, int scratch[]) {

    int i = lo;                     // walks the left run
    int j = mid + 1;                // walks the right run
    int k = lo;                     // walks the scratch array

    while ((i <= mid) && (j <= hi)) {
        if (a[i] <= a[j]) {
            scratch[k] = a[i];
            i++;
        } else {
            scratch[k] = a[j];
            j++;
        }
        k++;
    }

    while (i <= mid) {              // whatever is left of the left run
        scratch[k] = a[i];
        i++;
        k++;
    }

    while (j <= hi) {               // whatever is left of the right run
        scratch[k] = a[j];
        j++;
        k++;
    }

    for (k = lo; k <= hi; k++) {    // copy the merged result back
        a[k] = scratch[k];
    }
}

/* mergeSortRange       recursively merge sort one section of an array

Parameters:
a                       the array being sorted
lo                      first index of the section
hi                      last index of the section
scratch                 a work array at least as large as a

Returns:
nothing                 a[lo..hi] is left in ascending order
 */

void mergeSortRange (int a[], int lo, int hi, int scratch[]) {

    if (lo >= hi) {
        return;                                 // 0 or 1 element is sorted
    }

    int mid = lo + (hi - lo) / 2;               // written this way to avoid
                                                // overflow on large indices
    mergeSortRange (a, lo, mid, scratch);
    mergeSortRange (a, mid + 1, hi, scratch);
    merge (a, lo, mid, hi, scratch);
}

/* sortarray        sort an array of ints into ascending order. 
                    This is a Merge Sort.

Parameters:
a                   the array of ints to sort; it is modified in place
n                   the number of elements in the array

Returns:
nothing             the array a is left in ascending order


Merge Sort is used here on purpose. Every time enqueue happnens in a group, 
the array we are about to sort is already sorted except for the new values
at the end. That nearly sorted shape is the worst case for a quicksort
that picks the first element as its pivot, which would degrade to O(n^2) on 
exactly the input this program produces. Merge Sort is O(n log n) no matter 
what the data looks like.

The scratch array is allocated once per call rather than once per
merge, so the sort does one malloc regardless of how big n is. */

void sortarray (int a[], int n) {

    if (n < 2) {
        return;                                 // nothing to do
    }

    int *scratch = malloc (n * sizeof(int));
    if (scratch == NULL) {
        fprintf (stderr, "malloc failed, could not allocate sort scratch space\n");
        return;
    }

    mergeSortRange (a, 0, n - 1, scratch);
    free (scratch);
}

// Enqueue and Dequeue 

/* genarray             return an array of random ints on the heap

Parameters:
numberofelements        how many elements in the array

Returns:
a pointer to the new array, or NULL if the malloc failed


On Windows rand() only returns a 15-bit value (0 to 32767), so two 
calls are combined to build a wider random number. 
I need to free the array when I am finished! */

int *genarray (int numberofelements) {
    int *result = malloc (numberofelements * sizeof(int));
    if (result == NULL) {
        fprintf (stderr, "malloc failed\n");
    } else {
        // fill the result array with random numbers between 0 and MAX_VALUE
        for (int i=0; i<numberofelements; i++) {
            long long int r1 = rand();
            long long int r2 = rand();
            long long int randomValue = (r1 << 14) + (r2 & 0x3fff);
            result[i] = (int)(randomValue % MAX_VALUE);
        }
    }
    return (result);
}

/* pqEnqueueRandom      add a group of random numbers to the priority queue

Parameters:
pq                      pointer to the queue
numberofelements        how many random numbers to add

Returns:
the number of elements actually added, or 0 if the queue could not grow

The new values are appended to the end of the backing store and then
the whole array is sorted, which restores the sorted invariant.

This is the expensive half of the sorted-array priority queue. Adding
a group of k values to a queue that already holds m values costs O((m+k) log (m+k)) 
for the sort. */

int pqEnqueueRandom (struct pqueue *pq, int numberofelements) {

    if (numberofelements <= 0) {
        return (0);
    }

    if (!pqGrow (pq, pq->count + numberofelements)) {
        return (0);
    }

    int *newValues = genarray (numberofelements);
    if (newValues == NULL) {
        return (0);
    }

    for (int i=0; i<numberofelements; i++) {            // append to the end
        pq->data[pq->count + i] = newValues[i];
    }
    pq->count = pq->count + numberofelements;
    free (newValues);

    sortarray (pq->data, pq->count);                    // restore the order

    return (numberofelements);
}

/* pqDequeue    remove and return the lowest value in the priority queue

Parameters:
pq              pointer to the queue
value           where to store the value that was removed

Returns:
1 or 0          if a value was removed // if the queue was already empty


The array is kept sorted, so the highest-priority entry is always the
one at the front. After taking it, every remaining element is copied
down one slot so that the front of the queue is once again data[0].

That copy is what makes each individual dequeue O(n). Removing k
entries from a queue of n costs about O(k * n), this is the
expensive half of the operation and the reason the 10 second N for
removing is so much smaller than one might expect.*/

int pqDequeue (struct pqueue *pq, int *value) {

    if (pq->count == 0) {
        return (0);                             // queue is empty
    }

    *value = pq->data[0];                       // front = lowest value

    for (int i=0; i < pq->count - 1; i++) {     // shift everything down one
        pq->data[i] = pq->data[i + 1];
    }
    pq->count--;

    return (1);
}

/* main func.

main        alternate between adding random numbers and removing them

Hitting ENTER at either prompt (or Ctrl-D for end of file) stops the
program. A dequeue group stops early if the queue runs empty.

Only the first MAX_TO_PRINT removed values are printed */

int main (void) {

    char buffer[BUFFER_SIZE];
    struct pqueue pq;
    int keepgoing = 1;

    srand (time(NULL));                         // seed the random generator
    pqInit (&pq);

    while (keepgoing) {

        // Add
        printf ("How many numbers to add:  ");
        if (fgets (buffer, BUFFER_SIZE, stdin) == NULL) {
            printf ("\n");
            break;                              // end of file (Ctrl-D)
        }
        if (buffer[0] == '\n' || buffer[0] == '\r') {
            break;                              // just hit ENTER
        }

        int toAdd = atoi (buffer);
        if (toAdd < 0) {
            fprintf (stderr, "Cannot add a negative number of entries\n");
        } else if (toAdd > 0) {
            clock_t startTime = clock();
            int added = pqEnqueueRandom (&pq, toAdd);
            clock_t endTime = clock();
            if (added == 0) {
                fprintf (stderr, "Could not add %d entries\n", toAdd);
            } else {
                printf ("Added %d entries in %f seconds (queue size %d)\n",
                        added,
                        (double)(endTime - startTime) / CLOCKS_PER_SEC,
                        pq.count);
            }
        }

        // Remove 

        printf ("How many numbers to remove:  ");
        if (fgets (buffer, BUFFER_SIZE, stdin) == NULL) {
            printf ("\n");
            break;                              // end of file (Ctrl-D)
        }
        if (buffer[0] == '\n' || buffer[0] == '\r') {
            break;                              // just hit ENTER
        }

        int toRemove = atoi (buffer);
        if (toRemove < 0) {
            fprintf (stderr, "Cannot remove a negative number of entries\n");
        } else if (toRemove > 0) {
            int removed = 0;
            int value = 0;
            int printed = 0;

            clock_t startTime = clock();
            while ((removed < toRemove) && pqDequeue (&pq, &value)) {
                if (printed < MAX_TO_PRINT) {
                    printf ("%d ", value);
                    printed++;
                }
                removed++;
            }
            clock_t endTime = clock();

            if (printed > 0) {
                printf ("\n");
            }
            if (printed < removed) {
                printf ("... (%d more not shown)\n", removed - printed);
            }
            printf ("Removed %d entries in %f seconds (queue size %d)\n",
                    removed,
                    (double)(endTime - startTime) / CLOCKS_PER_SEC,
                    pq.count);

            if (removed < toRemove) {
                printf ("The priority queue is empty.\n");
                keepgoing = 0;
            }
        }
    }

    pqFree (&pq);
    return (0);
}