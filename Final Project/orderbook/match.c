/* This file is the implementation of the matching engine

The core idea: an incoming order that "crosses" (a buy priced at or
above the best ask, or a sell priced at or below the best bid, or any
market order) walks the opposing tree starting from its best price,
eating resting orders head-first until either the incoming order is
fully filled or nothing on the other side crosses anymore. 
Whatever is left of a LIMIT order then rests in its own tree; whatever is left
of a MARKET order is discarded (there is nobody left to trade with). */


#include <stdio.h>
#include <stdlib.h>
#include "match.h"

/* engineInit		puts the engine into a valid empty state
Parameters:
struct Engine *eng	the engine to initialize

Returns:
nothing (void) */
void engineInit(struct Engine *eng) {
    bookInit(&eng->book);
    eng->nextId = 1;
}

/* engineFree		frees everything the engine owns
Parameters:
struct Engine *eng	the engine to tear down

Returns:
nothing (void) */
void engineFree(struct Engine *eng) {
    bookFree(&eng->book);
}

/* sideName		returns a printable name for a side constant
Parameters:
int side	    SIDE_BUY or SIDE_SELL

Returns:
char *		    "BUY" or "SELL" (a string constant; do not free) */
char *sideName(int side) {
    if (side == SIDE_BUY) {
        return ("BUY");
    }
    return ("SELL");
}

/* matchAgainstBook	fills an incoming order against the opposing tree
Parameters:
struct Engine *eng	the engine (holds both trees)
int side		    side of the INCOMING order
long limitPrice		the incoming order's limit price, or -1 to
				    mean "market order: any price is acceptable"
long qty		    how much the incoming order wants
long incomingId		id of the incoming order (for the trade log)

Returns:
long	            how much of the incoming order is STILL unfilled

Notes:
This function is the whole reason min/max queries matter:
every loop iteration asks the opposing AVL tree for its best
price level (min of asks or max of bids), which is O(log n)
guaranteed. Emptied levels are deleted immediately so the
next iteration's min/max is again the true best price. */ 
long matchAgainstBook(struct Engine *eng, int side, long limitPrice,
                      long qty, long incomingId) {
    while (qty > 0) {
        // Step 1: find the best opposing price level.
        struct PriceLevel *best;
        if (side == SIDE_BUY) {
            best = levelMin(eng->book.asks);    // cheapest seller
        } else {
            best = levelMax(eng->book.bids);    // highest buyer
        }
        if (best == NULL) {
            break; // opposing side of the book is completely empty
        }

        /* Step 2: does the incoming order cross this price?
        A market order (limitPrice == -1) crosses everything.*/
        if (limitPrice >= 0) {
            if ((side == SIDE_BUY) && (best->price > limitPrice)) {
                break;  // cheapest ask is still above what user will pay
            }
            if ((side == SIDE_SELL) && (best->price < limitPrice)) {
                break;  // highest bid is still below what user will accept
            }
        }

        /* Step 3: eat resting orders at this level, oldest first,
        until the incoming order is full or the level is empty. */
        while ((qty > 0) && (!queueIsEmpty(&best->orders))) {
            struct Order *resting = best->orders.head;

            // fill size = the smaller of the two remaining quantities
            long fill = qty;
            if (resting->qty < fill) {
                fill = resting->qty;
            }

            /* the trade "prints", this line is where every price on a
            price chart comes from */
            printf("EXEC  %ld @ %ld  (incoming #%ld x resting #%ld)\n",
                   fill, best->price, incomingId, resting->id);

            qty = qty - fill;
            resting->qty = resting->qty - fill;
            best->totalQty = best->totalQty - fill;

            if (resting->qty == 0) {
                // resting order completely filled: remove and free it
                struct Order *done = queueDequeue(&best->orders);
                free(done);
            }
            /* if resting->qty > 0 the incoming order must be full
            (qty == 0), so both loops will end, a partial fill of
            the resting order leaves it at the head, keeping its
            time priority for the next incoming order. */
        }

        /* Step 4: if the level is now empty, delete it from the tree.
        Save the price first because levelDelete frees the node. */
        if (queueIsEmpty(&best->orders)) {
            long emptyPrice = best->price;
            if (side == SIDE_BUY) {
                eng->book.asks = levelDelete(eng->book.asks, emptyPrice);
            } else {
                eng->book.bids = levelDelete(eng->book.bids, emptyPrice);
            }
        }
    }
    return (qty);
}

/* engineSubmitLimit	processes a LIMIT BUY or LIMIT SELL event
Parameters:
struct Engine *eng	    the engine
int side		        SIDE_BUY or SIDE_SELL
long price		        limit price in integer ticks
long qty		        number of contracts wanted

Returns:
long	                the id assigned to this order, or -1 on error

Notes:
A limit order may cross (trade immediately), rest entirely, or do
some of each (partial fill, remainder rests). Only the UNFILLED remainder is 
stored in the book. */
long engineSubmitLimit(struct Engine *eng, int side, long price, long qty) {
    if ((qty <= 0) || (price <= 0)) {
        printf("ERROR: limit order needs positive price and qty\n");
        return (-1);
    }
    long id = eng->nextId;
    eng->nextId = eng->nextId + 1;
    printf("ORDER #%ld  LIMIT %s %ld @ %ld\n", id, sideName(side), qty, price);

    // first, trade against anything on the other side that crosses
    long remaining = matchAgainstBook(eng, side, price, qty, id);

    if (remaining == 0) {
        return (id); // fully filled on arrival; nothing rests
    }

    /* rest the remainder: find (or create) the price level, then join
    the back of its FIFO queue */
    struct PriceLevel *level = NULL;
    if (side == SIDE_BUY) {
        eng->book.bids = levelInsert(eng->book.bids, price, &level);
    } else {
        eng->book.asks = levelInsert(eng->book.asks, price, &level);
    }
    if (level == NULL) {
        return (-1); // malloc failed inside levelInsert (already reported)
    }

    struct Order *o = orderCreate(id, side, price, remaining);
    if (o == NULL) {
        return (-1);
    }
    queueEnqueue(&level->orders, o);
    level->totalQty = level->totalQty + remaining;

    printf("REST  #%ld  %s %ld @ %ld\n", id, sideName(side), remaining, price);
    return (id);
}

/* engineSubmitMarket	processes a MARKET BUY or MARKET SELL event
Parameters:
struct Engine *eng	    the engine
int side		        SIDE_BUY or SIDE_SELL
long qty		        number of contracts wanted

Returns:
long	                the id assigned to this order, or -1 on error

Notes:
limitPrice of -1 tells the matcher to accept ANY price. 
A market order never rests: if the opposing side runs out, the remainder is
simply reported as unfilled (this is what a real exchange does when
one side of the book is swept clean). */
long engineSubmitMarket(struct Engine *eng, int side, long qty) {
    if (qty <= 0) {
        printf("ERROR: market order needs positive qty\n");
        return (-1);
    }
    long id = eng->nextId;
    eng->nextId = eng->nextId + 1;
    printf("ORDER #%ld  MARKET %s %ld\n", id, sideName(side), qty);

    long remaining = matchAgainstBook(eng, side, -1, qty, id);
    if (remaining > 0) {
        printf("NOTE  #%ld  %ld unfilled -- %s side of book is empty\n",
               id, remaining, (side == SIDE_BUY) ? "ask" : "bid");
    }
    return (id);
}

/* engineCancel		processes a CANCEL event
Parameters:
struct Engine *eng	the engine
int side		    side the order is resting on
long price		    price level the order is resting at
long id			    the order id to cancel

Returns:
int	0 if the order was found and cancelled, -1 if not

Notes:
Cost breakdown, described in my proposal: an O(log n)
AVL descent to the price level, then an O(k) scan of that one
level's queue (k = orders at that price), NOT a scan of the whole
book. 
If the cancel empties the level, the level is deleted so the tree 
never holds hollow nodes. */
int engineCancel(struct Engine *eng, int side, long price, long id) {
    struct PriceLevel *level;
    if (side == SIDE_BUY) {
        level = levelFind(eng->book.bids, price);
    } else {
        level = levelFind(eng->book.asks, price);
    }
    if (level == NULL) {
        printf("CANCEL #%ld FAILED: no %s level at %ld\n",
               id, sideName(side), price);
        return (-1);
    }

    long removedQty = 0;
    if (queueRemoveById(&level->orders, id, &removedQty) != 0) {
        printf("CANCEL #%ld FAILED: not resting at %s %ld\n",
               id, sideName(side), price);
        return (-1);
    }
    level->totalQty = level->totalQty - removedQty;
    printf("CANCEL #%ld  removed %ld @ %ld\n", id, removedQty, price);

    if (queueIsEmpty(&level->orders)) {
        if (side == SIDE_BUY) {
            eng->book.bids = levelDelete(eng->book.bids, price);
        } else {
            eng->book.asks = levelDelete(eng->book.asks, price);
        }
    }
    return (0);
}
