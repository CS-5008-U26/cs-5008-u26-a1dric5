/* Priority Queue Using A Heap (Task 2)
 
The same priority queue as pqarray.c, but the backing store is a binary
min-heap instead of a fully sorted array.
 
Definitions to know (for myself!): 
ENQUEUE: append the new value at the end of the array, then heapifyUp
from that position. One value at a time, O(log n) each.

DEQUEUE: exchange the root with the last element, reduce the heap size
by one, then heapifyDown on the root. This is exactly one
step of Heapsort. O(log n).

The heap is stored in an array.
the children of i are at 2i+1 and 2i+2, and the parent of i is at
(i-1)/2. No pointers, no wasted slot at index 0.

MIN-HEAP -- the heap property here is that a parent is always less than or
equal to both of its children, so the smallest element sits at the root. 
The only difference from a max-heap is the direction of the comparisons in 
heapifyUp and heapifyDown.

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


// Priority Queue 

/*struct pqueue   a priority queue backed by a binary min-heap

Fields:
data            the backing store, a heap array satisfying the heap property
count           how many elements are actually in the heap right now
capacity        how many elements the backing store can hold before it
                has to be made bigger

The invariant is the heap property, not full sortedness. 
for every index i > 0, data[(i-1)/2] <= data[i].

The array is not in ascending order and printing it wouldn't be understandable. 
But it is still enough to guarantee that data[0] is the smallest element in the 
whole heap, which is all a priority queue actually needs to know. Maintaining 
the weaker invariant is what makes both operations cheap. */

struct pqueue {
    int *data;
    int count;
    int capacity;
};

/* pqInit       set up an empty priority queue

Parameters:
pq              pointer to the queue to initialize

Returns         nothing

No memory is allocated here. The first enqueue allocates the backing
store, so a queue that is never used costs nothing.*/

void pqInit (struct pqueue *pq) {
    pq->data = NULL;
    pq->count = 0;
    pq->capacity = 0;
}

/* pqFree       release the backing store of a priority queue

Parameters:
pq              pointer to the queue to free

Returns         nothing

The queue is left in the same state as a freshly initialized one, so
it is safe to call pqFree twice or to keep using the queue afterward.*/

void pqFree (struct pqueue *pq) {
    free (pq->data);
    pq->data = NULL;
    pq->count = 0;
    pq->capacity = 0;
}

/* pqGrow       make sure the backing store can hold at least "needed" elements

Parameters:
pq              pointer to the queue
needed          the number of elements the backing store must be able to hold

Returns:
1 or 0          if the backing store is big enough // if the reallocation failed


The capacity is doubled each time it is too small, rather than grown by
a fixed amount. Doubling means that adding n elements one at a time
costs O(n) copying in total instead of O(n^2), because each element
gets copied only a constant number of times on average.
 
If doubling still is not enough (someone asked for a huge group all at
once), the capacity jumps straight to what was asked for.*/

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

// Heap operations

/* swap     exchange two elements of an array

Parameters:
a           the array
i           index of the first element
j           index of the second element

Returns     nothing */

void swap (int a[], int i, int j) {
    int temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

/* heapifyUp    restore the heap property by moving one element toward the root
 
Parameters:
a               the heap array
index           the position of the element that may be out of place

Returns:
nothing         the heap property holds when this returns


Used after appending a new value at the end of the heap. The new value
is compared with its parent and swapped upward as long as it is smaller
than the parent. In a max-heap this comparison would be reversed; that
one line is the entire difference between a min-heap and a max-heap.

The element can travel at most the height of the heap, so this is
O(log n). */

void heapifyUp (int a[], int index) {

    while (index > 0) {
        int parent = (index - 1) / 2;
        if (a[index] < a[parent]) {             // MIN-heap: smaller goes up
            swap (a, index, parent);
            index = parent;
        } else {
            break;                              // parent is already smaller
        }
    }
}

/* heapifyDown      restore the heap property by moving one element toward the leaves

Parameters:
a                   the heap array
n                   how many elements are in the heap
index               the position of the element that may be out of place

Returns:
nothing             the heap property holds when this returns


Used after a dequeue puts the former last element at the root. The
element is compared with both children, and swapped with the smaller of
the two if that child is smaller than the element itself. In a max-heap
the larger child would be found instead.

Comparing against the smaller child is what keeps the property intact, 
swapping with the larger child could leave the new parent bigger than
its remaining sibling.

The element can travel at most the height of the heap, so this is
O(log n). */

void heapifyDown (int a[], int n, int index) {

    int keepgoing = 1;

    while (keepgoing) {
        int left = (2 * index) + 1;
        int right = (2 * index) + 2;
        int smallest = index;

        if ((left < n) && (a[left] < a[smallest])) {
            smallest = left;
        }
        if ((right < n) && (a[right] < a[smallest])) {
            smallest = right;
        }

        if (smallest == index) {
            keepgoing = 0;                      // both children are bigger
        } else {
            swap (a, index, smallest);
            index = smallest;
        }
    }
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

/* pqEnqueue    add one value to the priority queue

Parameters:
pq              pointer to the queue
value           the value to add

Returns:
1 or 0          if the value was added // if the queue could not grow


The value goes at the end of the array, the next open leaf position, 
and then heapifyUp walks it toward the root until the heap property
holds again. O(log n) per value.
 */

int pqEnqueue (struct pqueue *pq, int value) {

    if (!pqGrow (pq, pq->count + 1)) {
        return (0);
    }

    pq->data[pq->count] = value;                // append at the end
    pq->count++;
    heapifyUp (pq->data, pq->count - 1);        // walk it up into place

    return (1);
}

/* pqEnqueueRandom      add a group of random numbers to the priority queue

Parameters:
pq                      pointer to the queue
numberofelements        how many random numbers to add

Returns:
the number of elements actually added, or 0 if the queue could not grow


Each value is inserted individually with pqEnqueue (as the assignment
specifies). Adding a group of k value costs O(k log n).

The capacity is grown once up front rather than letting each insert
check for itself, so a big group does one realloc instead of many.*/

int pqEnqueueRandom (struct pqueue *pq, int numberofelements) {

    if (numberofelements <= 0) {
        return (0);   }

    if (!pqGrow (pq, pq->count + numberofelements)) {
     return (0);
    }

    int *newValues = genarray (numberofelements);
    if (newValues == NULL) {
        return (0);
    }

    for (int i=0; i<numberofelements; i++) {
        pqEnqueue (pq, newValues[i]);           // one at a time, heapifyUp each
    }

    free (newValues);
    return (numberofelements);
}

/* pqDequeue    remove and return the lowest value in the priority queue

Parameters:
pq              pointer to the queue
value           where to store the value that was removed

Returns:
1 or 0          if a value was removed // if the queue was already empty

This is exactly one step of Heapsort:
1. the root is the smallest value, so save it
2. exchange the root with the last element of the heap
3. reduce the heap size by one, which drops the old root out
4. heapifyDown on the new root to restore the heap property  

What's not in here: no shifting of the remaining elements. That
O(n) copy loop was the whole cost of dequeue in pqarray.c. Here the
work is one swap plus a walk down the height of the heap, so O(log n). */

int pqDequeue (struct pqueue *pq, int *value) {

    if (pq->count == 0) {
        return (0);                             // queue is empty
    }

    *value = pq->data[0];                       // root = lowest value

    swap (pq->data, 0, pq->count - 1);          // exchange root with last
    pq->count--;                                // shrink the heap
    heapifyDown (pq->data, pq->count, 0);       // sift the new root down

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