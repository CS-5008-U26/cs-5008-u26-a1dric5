/* This file is the implementation of the Order struct and its FIFO queue

This module is "just" the linked-list material from class:
malloc a node, hook up pointers, free when done.  

The only thing to note is that I kept a tail pointer so enqueue is O(1) 
instead of O(k). */


#include <stdio.h>
#include <stdlib.h>
#include "order.h"

/* orderCreate		allocates and fills in one new Order
Parameters:
long id		        the unique id to give the order
int side	        SIDE_BUY or SIDE_SELL
long price	        price in integer ticks
long qty	        number of contracts

Returns:
struct Order *	    pointer to the new order, or NULL if malloc failed

Notes:
the lowest-level function prints the error message; the caller just checks for NULL. */
struct Order *orderCreate(long id, int side, long price, long qty) {
    struct Order *o = malloc(sizeof(struct Order));
    if (o == NULL) {
        printf("ERROR: out of memory creating order %ld\n", id);
        return (NULL);
    }
    o->id = id;
    o->side = side;
    o->price = price;
    o->qty = qty;
    o->next = NULL;   // a brand new order is always the last in its queue
    return (o);
}

/* queueInit		    puts a queue into a valid empty state
Parameters:
struct OrderQueue *q	the queue to initialize

Returns:
nothing (void)

Notes:
Must be called before any other queue function is used, otherwise 
head/tail contain garbage values (uninitialized memory). */
void queueInit(struct OrderQueue *q) {
    q->head = NULL;
    q->tail = NULL;
}

/* queueIsEmpty		    tells whether a queue has no orders in it
Parameters:
struct OrderQueue *q	the queue to examine

Returns:
int (bool)	            1 if empty, 0 if it holds at least one order */
int queueIsEmpty(struct OrderQueue *q) {
    return (q->head == NULL);
}

/* queueEnqueue		    adds an order at the TAIL of the queue (newest position)
Parameters:
struct OrderQueue *q	the queue to add to
struct Order *o		    the order being added

Returns:
nothing (void)

Notes:
This is where time priority is created: 
a new order always goes behind every order that arrived before it at the same price. */
void queueEnqueue(struct OrderQueue *q, struct Order *o) {
    o->next = NULL;
    if (q->tail == NULL) {
        // queue was empty: the new order is both head and tail
        q->head = o;
        q->tail = o;
    } else {
        // hook the new order behind the current tail, then move the tail
        q->tail->next = o;
        q->tail = o;
    }
}

/*queueDequeue		    removes and returns the order at the HEAD (oldest)
Parameters:
struct OrderQueue *q	the queue to remove from

Returns:
struct Order *	        the oldest order, or NULL if the queue is empty

Notes:
The caller now owns the returned order and must free() it when finished with it. */
struct Order *queueDequeue(struct OrderQueue *q) {
    if (q->head == NULL) {
        return (NULL);
    }
    struct Order *o = q->head;
    q->head = o->next;
    if (q->head == NULL) {
        // removed the last order, so there is no tail anymore either
        q->tail = NULL;
    }
    o->next = NULL; // detach it fully from the list
    return (o);
}

/* queueRemoveById	    finds an order by id, unlinks it, and frees it
Parameters:
struct OrderQueue *q	the queue to search
long id			        the id of the order to cancel
long *removedQty	    OUT parameter: set to the cancelled order's
				        remaining qty, so the caller can subtract it
				        from the price level's totalQty
Returns:
int	0 if the order was found and removed, -1 if not found

Notes:
This is the "delete from a singly linked list" pattern:
traverse with two pointers (prev and cur) so that when we find the
node, prev->next can be re-routed around it. 
The out-parameter is the pass-a-pointer trick for returning a second value. */
int queueRemoveById(struct OrderQueue *q, long id, long *removedQty) {
    struct Order *prev = NULL;
    struct Order *cur = q->head;
    *removedQty = 0;
    while (cur != NULL) {
        if (cur->id == id) {
            *removedQty = cur->qty;
            // unlink cur from the chain
            if (prev == NULL) {
                q->head = cur->next; // cur was the head
            } else {
                prev->next = cur->next;
            }
            if (q->tail == cur) {
                q->tail = prev; // cur was the tail; prev is the new tail
            }
            free(cur);
            return (0);
        }
        prev = cur;
        cur = cur->next;
    }
    return (-1); // walked the whole list, id not present at this level
}

/* queueFreeAll		    frees every order still in the queue
Parameters:
struct OrderQueue *q	the queue to empty

Returns:
nothing (void)

Notes:
must save cur->next BEFORE freeing cur, because reading a field
out of freed memory is undefined behavior (ASan would flag it). */
void queueFreeAll(struct OrderQueue *q) {
    struct Order *cur = q->head;
    while (cur != NULL) {
        struct Order *nextOne = cur->next;
        free(cur);
        cur = nextOne;
    }
    q->head = NULL;
    q->tail = NULL;
}
