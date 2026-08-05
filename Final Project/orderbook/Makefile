# Makefile for the Limit Order Book / Matching Engine

# Targets:
# 	make			build the orderbook program
#	make test       build and run the AVL test harness
# 	make demo       run the sample input file
#	make sim        run the simulator with a strong up trend (seeded)
# 	make clean      remove built files

# The flags follow the proposal: 
# warnings on, debug info, and AddressSanitizer 
# so use-after-free / leaks are caught while testing.

CC = clang
CFLAGS = -Wall -Wextra -g -fsanitize=address

orderbook: main.c match.c book.c order.c match.h book.h order.h
	$(CC) $(CFLAGS) -o orderbook main.c match.c book.c order.c

test_avl: test_avl.c book.c order.c book.h order.h
	$(CC) $(CFLAGS) -o test_avl test_avl.c book.c order.c

test: test_avl
	./test_avl

demo: orderbook
	./orderbook sample_events.txt

sim: orderbook
	./orderbook --sim 500 80 42

clean:
	rm -f orderbook test_avl
	rm -rf orderbook.dSYM test_avl.dSYM
