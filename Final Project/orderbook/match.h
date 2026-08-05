/* This file is the matching engine: turns incoming events into trades

The engine owns the book plus a counter that hands out order ids.
The rules it enforces are the two real exchange priority rules:

Price priority: an incoming buy matches the LOWEST ask first
                (and an incoming sell matches the HIGHEST bid first).

Time priority: within one price level, the OLDEST resting order
                is filled first (the head of the FIFO queue). */


#ifndef MATCH_H
#define MATCH_H

#include "book.h"

// The engine: the book plus the next order id to hand out.
struct Engine {
    struct Book book;
    long nextId;        // ids are assigned 1, 2, 3, ... in arrival order
};

// See match.c for the opening comment of each function.
void engineInit(struct Engine *eng);
void engineFree(struct Engine *eng);
long engineSubmitLimit(struct Engine *eng, int side, long price, long qty);
long engineSubmitMarket(struct Engine *eng, int side, long qty);
int engineCancel(struct Engine *eng, int side, long price, long id);

#endif
