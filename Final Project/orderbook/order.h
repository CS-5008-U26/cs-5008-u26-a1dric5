/* This file is the Order struct and the FIFO queue of orders at one price level

Every resting order in the book is one Order struct.  
Orders that share the same price are kept in a singly linked list, oldest first.  
That list is the "time priority" rule: matching always consumes from the head,
so the order that arrived first is filled first */

#ifndef ORDER_H
#define ORDER_H

/* Which side of the book an order belongs to use plain int constants (not enum) 
to keep it simple */
#define SIDE_BUY 0
#define SIDE_SELL 1

/* One resting order in the book

"next" makes this a linked-list node, except the data fields describe an 
order instead of a number. */
struct Order {
    long id;            // unique order id, assigned by the program
    int side;           // SIDE_BUY or SIDE_SELL
    long price;         // price in integer ticks (real exchanges do this too)
    long qty;           // how many contracts are still unfilled
    struct Order *next; // next (younger) order at the same price, or NULL
};

/* The FIFO queue of orders resting at one price.
keep both a head and a tail pointer:
    head -> where matching removes orders (dequeue is O(1))
    tail -> where new orders are added (enqueue is O(1))

Without the tail pointer, enqueue would have to walk the whole list = O(k) */
struct OrderQueue {
    struct Order *head; // oldest order (first to be filled), or NULL if empty
    struct Order *tail; // newest order (last to be filled), or NULL if empty
};

// See order.c for the opening comment of each function.
struct Order *orderCreate(long id, int side, long price, long qty);
void queueInit(struct OrderQueue *q);
void queueEnqueue(struct OrderQueue *q, struct Order *o);
struct Order *queueDequeue(struct OrderQueue *q);
int queueRemoveById(struct OrderQueue *q, long id, long *removedQty);
void queueFreeAll(struct OrderQueue *q);
int queueIsEmpty(struct OrderQueue *q);

#endif
