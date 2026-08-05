# Limit Order Book / Matching Engine in C Using AVL Trees

CS 5008 final project — Aldrich Gran Lapid, Summer 2026.

A command-line C program (C standard library only) that maintains a two-sided limit order book for one instrument, matches incoming orders with **price-time priority**, and keeps each side of the book in an **AVL tree keyed by integer price ticks** so every operation is guaranteed O(log n) — even on trending (near-sorted) order flow, the exact input that degrades a plain BST to O(n).

## Files

File -- What it is:
`order.h` / `order.c` --  The `Order` struct and the FIFO queue at each price level (time priority)
`book.h` / `book.c` -- The `PriceLevel` AVL tree (insert delete, all four rotations, min/max, invariant checker) and the two-sided `Book`
`match.h` / `match.c` -- The matching engine: limit orders, market orders, partial fills, cancels, level deletion
`main.c` -- Input-file parser and the built-in random-walk simulator
`test_avl.c` -- Test harness: sorted + shuffled inserts and full deletion, asserting balance/ordering invariants after **every** operation
`sample_events.txt` -- Scripted demo of every event type

## Build

Requires clang (or gcc — run `make CC=gcc ...`).
All builds use `-Wall -Wextra -g -fsanitize=address`, so use-after-free and memory leaks are caught automatically during every run.

```
make            # builds ./orderbook
make test       # builds and runs the AVL test harness
```

## Run

**From an event file:**

```
./orderbook sample_events.txt
```

Event file format (one event per line; `#` starts a comment):

```
LIMIT BUY <price> <qty>
LIMIT SELL <price> <qty>
MARKET BUY <qty>
MARKET SELL <qty>
CANCEL BUY <price> <id>      # side + price find the level (O(log n)),
CANCEL SELL <price> <id>     # the id finds the order in that level
BOOK [depth]                 # print the top of the book (default 5)
```

Prices are integer ticks (as in real systems). Order ids are assigned
1, 2, 3, ... in arrival order and printed when each order is accepted.

**From the simulator:**

```
./orderbook --sim <numEvents> <trend> [seed]
```

`trend` is −100..100: 0 is a pure random walk, 90 trends strongly up.
A strong trend feeds the trees near-sorted prices; the end-of-run
report prints each tree's node count, height, and an invariant check,
so you can see the AVL height staying near log2(n).

```
make sim        # 500 events, trend 80, seed 42 (reproducible)
```

## Complexity (n = number of price levels, k = orders at one level)

- Insert new price level: O(log n) plus rebalancing rotations
- Find best bid/ask: O(log n) (max of bid tree / min of ask tree)
- Delete emptied level: O(log n)
- Enqueue/dequeue an order at a level: O(1)
- Cancel by id: O(log n) descent + O(k) scan of one level's queue
