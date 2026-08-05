// This file is the PriceLevel AVL tree and the two-sided order book

/* Design (matches Figure 2 of my proposal):
   * Each AVL tree node is one price level.  The node's key is the price
     in integer ticks, and the node carries a FIFO queue of the orders
     resting at that price.
   * The book holds two trees: one for bids (buys) and one for asks (sells).
   * Best bid  = the MAXIMUM key in the bid tree (highest buyer).
     Best ask  = the MINIMUM key in the ask tree (lowest seller).

Why AVL and not a plain BST?  
Prices trend, so insertions arrive in nearly sorted order, exactly the input 
that collapses a plain BST into a linked list (O(n) per operation).  
The AVL height invariant (left and right subtree heights differ by at most 1) 
keeps the tree height near log2(n) no matter what order the prices arrive in. */


#ifndef BOOK_H
#define BOOK_H

#include "order.h"

// One node of the AVL tree = one price level in the book.
struct PriceLevel {
    long price;               // the key: price in integer ticks
    long totalQty;            // sum of qty of all resting orders here
    struct OrderQueue orders; // FIFO queue = time priority at this price
    int height;               // AVL bookkeeping: height of this subtree
    struct PriceLevel *left;  // prices smaller than this one
    struct PriceLevel *right; // prices larger than this one
};

// The whole order book: a bid tree and an ask tree.
struct Book {
    struct PriceLevel *bids; // root of the BUY-side AVL tree
    struct PriceLevel *asks; // root of the SELL-side AVL tree
};

// See book.c for the opening comment of each function.
void bookInit(struct Book *book);
void bookFree(struct Book *book);

struct PriceLevel *levelInsert(struct PriceLevel *root, long price,
                               struct PriceLevel **found);
struct PriceLevel *levelDelete(struct PriceLevel *root, long price);
struct PriceLevel *levelFind(struct PriceLevel *root, long price);
struct PriceLevel *levelMin(struct PriceLevel *root);
struct PriceLevel *levelMax(struct PriceLevel *root);

int levelHeight(struct PriceLevel *node);
int levelCheckInvariants(struct PriceLevel *root, long low, long high);

void bookDisplay(struct Book *book, int depth);

#endif
