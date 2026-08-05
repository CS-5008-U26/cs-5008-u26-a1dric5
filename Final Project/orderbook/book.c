/* This file is the AVL tree of price levels and the two-sided book

This is the heart of the project.  

The functions below are a normal recursive binary search tree 
(insert / delete / find / min / max)

Plus the AVL rebalancing step: after every insert or delete, update
the node's height, measure the "balance factor" (left height minus
right height), and if it is outside the range [-1, +1], perform one
of four rotation cases to restore the invariant.

Rotation notes (the four cases):
Left-Left   (balance > +1, new key went left-left)    -> rotate right
Right-Right (balance < -1, new key went right-right)  -> rotate left
Left-Right  (balance > +1, new key went left-right)   -> rotate left
	                                    on child, then rotate right
Right-Left  (balance < -1, new key went right-left)   -> rotate right
	                                    on child, then rotate left */

#include <stdio.h>
#include <stdlib.h>
#include "book.h"

/* Small helper functions
levelHeight		            returns the stored height of a subtree

Parameters:
struct PriceLevel *node		root of the subtree (may be NULL)

Returns:
int	                        the height of the subtree; an empty tree has height 0

Notes:
Treating NULL as height 0 means every other function can call this
without first checking for NULL itself.  A single leaf has height 1. */
int levelHeight(struct PriceLevel *node) {
    if (node == NULL) {
        return (0);
    }
    return (node->height);
}

/* maxInt		returns the larger of two ints

Parameters:
int a	first value
int b	second value

Returns:
int	whichever of a and b is larger */
int maxInt(int a, int b) {
    if (a > b) {
        return (a);
    }
    return (b);
}

/* levelBalance		        computes the balance factor of a node
Parameters:
struct PriceLevel *node		the node to measure (may be NULL)

Returns:
int	left subtree height minus right subtree height

Notes:
The AVL invariant says this must always be -1, 0, or +1.
+2 means the left side is too tall; -2 means the right side is. */
int levelBalance(struct PriceLevel *node) {
    if (node == NULL) {
        return (0);
    }
    return (levelHeight(node->left) - levelHeight(node->right));
}

/*levelUpdateHeight	        recomputes a node's height from its children
Parameters:
struct PriceLevel *node		the node to update

Returns:
nothing (void)*/

void levelUpdateHeight(struct PriceLevel *node) {
    node->height = 1 + maxInt(levelHeight(node->left),
                              levelHeight(node->right));
}

/* Rotations

rotateRight		        performs a right rotation around node y
Parameters:
struct PriceLevel *y	the unbalanced node (too tall on the left)

Returns:
struct PriceLevel *	    the new root of this subtree (was y's left child)

Notes:
	Before:        y                After:      x
	              / \  .                       / \  .
	             x   C                        A   y
	            / \  .                           / \  .
	           A   B                            B   C
	(the lone dots just stop the compiler treating \ as a line continuation)
	The BST ordering A < x < B < y < C is true both before and after,
	which is why a rotation is legal: it changes shape, never order.*/
struct PriceLevel *rotateRight(struct PriceLevel *y) {
    struct PriceLevel *x = y->left;
    struct PriceLevel *B = x->right;

    // re-hang the pointers
    x->right = y;
    y->left = B;

    // y is now below x, so y's height must be fixed first
    levelUpdateHeight(y);
    levelUpdateHeight(x);

    return (x); // x has taken y's old place
}

/* rotateLeft		    performs a left rotation around node x
Parameters:
struct PriceLevel *x	the unbalanced node (too tall on the right)

Returns:
struct PriceLevel *	    the new root of this subtree (was x's right child)

Notes:
This is the exact mirror image of rotateRight. */
struct PriceLevel *rotateLeft(struct PriceLevel *x) {
    struct PriceLevel *y = x->right;
    struct PriceLevel *B = y->left;

    y->left = x;
    x->right = B;

    levelUpdateHeight(x);
    levelUpdateHeight(y);

    return (y);
}

/* levelRebalance	        checks one node and fixes it if it violates AVL
Parameters:
struct PriceLevel *node		the node to check (not NULL)

Returns:
struct PriceLevel *	        the (possibly new) root of this subtree

Notes:
Called on the way back up from every recursive insert and delete.
This is what makes the tree "self-balancing": each individual call
does at most two rotations, and because the recursion visits only
the nodes on the path from the root down to the change, the total
work stays O(log n). */
struct PriceLevel *levelRebalance(struct PriceLevel *node) {
    levelUpdateHeight(node);
    int balance = levelBalance(node);

    /* Case 1: Left-Left.  
    Left side too tall, and the left child leans left (or is even).  
    One right rotation fixes it.*/ 
    if ((balance > 1) && (levelBalance(node->left) >= 0)) {
        return (rotateRight(node));
    }

    /* Case 2: Left-Right.  
    Left side too tall, but the left child leans RIGHT. 
    A single right rotation would not fix that, so first rotate the child 
    left (turning it into a Left-Left shape), then rotate this node right.*/
    if ((balance > 1) && (levelBalance(node->left) < 0)) {
        node->left = rotateLeft(node->left);
        return (rotateRight(node));
    }

    // Case 3: Right-Right.  Mirror of case 1.
    if ((balance < -1) && (levelBalance(node->right) <= 0)) {
        return (rotateLeft(node));
    }

    // Case 4: Right-Left.  Mirror of case 2.
    if ((balance < -1) && (levelBalance(node->right) > 0)) {
        node->right = rotateRight(node->right);
        return (rotateLeft(node));
    }

    // Balance factor is -1, 0, or +1: nothing to do.
    return (node);
}

/* Insert / find / min / max / delete

levelCreate		        allocates one new, empty price level
Parameters:
long price	            the price this level represents

Returns:
struct PriceLevel *	    the new node, or NULL if malloc failed */
struct PriceLevel *levelCreate(long price) {
    struct PriceLevel *node = malloc(sizeof(struct PriceLevel));
    if (node == NULL) {
        printf("ERROR: out of memory creating price level %ld\n", price);
        return (NULL);
    }
    node->price = price;
    node->totalQty = 0;
    queueInit(&node->orders);
    node->height = 1;           // a new node is a leaf, and a leaf has height 1
    node->left = NULL;
    node->right = NULL;
    return (node);
}

/*levelInsert		        inserts a price into the tree (or finds it if present)
Parameters:
struct PriceLevel *root		root of the subtree to insert into
long price			        the price to insert
struct PriceLevel **found	OUT parameter: set to point at the
					        level for this price (new or existing)
Returns:
struct PriceLevel *	        the new root of this subtree (rotations can
				            change which node is on top!)
Notes:
This is the standard recursive BST insert from class, with two
additions: 
1) the *found out-parameter, using the pass-a-pointer
trick so the caller learns where the level is
2) the call to
levelRebalance on the way back up

The caller must always write:  root = levelInsert(root, p, &lvl);
because the returned pointer may differ from the old root. */
struct PriceLevel *levelInsert(struct PriceLevel *root, long price,
                               struct PriceLevel **found) {
    if (root == NULL) {
        // empty spot: this is where the new price belongs
        struct PriceLevel *node = levelCreate(price);
        *found = node;  // tell the caller about it (may be NULL on failure)
        return (node);
    }
    if (price < root->price) {
        root->left = levelInsert(root->left, price, found);
    } else if (price > root->price) {
        root->right = levelInsert(root->right, price, found);
    } else {
        // the price level already exists: no new node needed
        *found = root;
        return (root);
    }
    // may have made one side taller: restore the AVL invariant
    return (levelRebalance(root));
}

/* levelFind		        looks up a price in the tree
Parameters:
struct PriceLevel *root		root of the subtree to search
long price			        the price to look for

Returns:
struct PriceLevel *	        the level with that price, or NULL if absent

Notes:
Ordinary BST search: go left for smaller, right for larger.
Because the tree is AVL-balanced this is guaranteed O(log n). */ 
struct PriceLevel *levelFind(struct PriceLevel *root, long price) {
    while (root != NULL) {
        if (price < root->price) {
            root = root->left;
        } else if (price > root->price) {
            root = root->right;
        } else {
            return (root);
        }
    }
    return (NULL);
}

/* levelMin		            finds the smallest price in the tree
Parameters:
struct PriceLevel *root		root of the tree (may be NULL)

Returns:
struct PriceLevel *	        the leftmost node, or NULL if tree is empty

Notes:
The minimum of a BST is always the leftmost node. For the ASK
tree, this is the BEST ASK (the cheapest seller). */
struct PriceLevel *levelMin(struct PriceLevel *root) {
    if (root == NULL) {
        return (NULL);
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return (root);
}

/* levelMax		            finds the largest price in the tree
Parameters:
struct PriceLevel *root		root of the tree (may be NULL)

Returns:
struct PriceLevel *	        the rightmost node, or NULL if tree is empty

Notes:
For the BID tree, the maximum is the BEST BID (the highest buyer). */
struct PriceLevel *levelMax(struct PriceLevel *root) {
    if (root == NULL) {
        return (NULL);
    }
    while (root->right != NULL) {
        root = root->right;
    }
    return (root);
}

/* levelDelete		        removes a price level from the tree
Parameters:
struct PriceLevel *root		root of the subtree to delete from
long price			        the price of the level to remove

Returns:
struct PriceLevel *	        the new root of this subtree

Notes:
IMPORTANT: the caller must empty the level's order queue BEFORE
calling this; this function frees only the node struct itself.
The three BST delete cases:
    a) no children  -> just free the node
	b) one child    -> the child takes the node's place
	c) two children -> copy the in-order successor's data into this
node, then delete the successor from the right subtree.

After the structural change, levelRebalance runs on the way back up 
this is the part a plain BST does not do, and it is why AVL delete stays O(log n). */
struct PriceLevel *levelDelete(struct PriceLevel *root, long price) {
    if (root == NULL) {
        return (NULL);      // price not in tree: nothing to do
    }
    if (price < root->price) {
        root->left = levelDelete(root->left, price);
    } else if (price > root->price) {
        root->right = levelDelete(root->right, price);
    } else {
        // found the node to delete
        if ((root->left == NULL) || (root->right == NULL)) {
            // cases (a) and (b): at most one child
            struct PriceLevel *child = root->left;
            if (child == NULL) {
                child = root->right;
            }
            free(root);
            return (child);     // child (or NULL) replaces the node
        }
        /* case (c): two children.  
        Find the in-order successor = the SMALLEST price in the RIGHT subtree.  
        It is the next price up, so copying its data here keeps the BST ordering correct.*/
        struct PriceLevel *succ = levelMin(root->right); 
        root->price = succ->price;
        root->totalQty = succ->totalQty;
        root->orders = succ->orders; // struct copy: takes over the queue
        /* Now the successor node is a duplicate shell; remove it from
        the right subtree. Its orders were taken over above, so the
        free() inside the recursive call only frees the shell. */
        root->right = levelDelete(root->right, succ->price);
    }
    return (levelRebalance(root));
}

// Whole-book functions

/* bookInit		    puts a Book into a valid empty state
Parameters:
struct Book *book	the book to initialize

Returns:
nothing (void) */
void bookInit(struct Book *book) {
    book->bids = NULL;
    book->asks = NULL;
}

/* treeFreeAll		        frees every node and every order in a subtree
Parameters:
struct PriceLevel *root		root of the subtree to free

Returns:
nothing (void)

Notes:
Post-order traversal: free both children FIRST, then the node,
because once the node is freed we could no longer reach them. */
void treeFreeAll(struct PriceLevel *root) {
    if (root == NULL) {
        return;
    }
    treeFreeAll(root->left);
    treeFreeAll(root->right);
    queueFreeAll(&root->orders);    // free the resting orders at this level
    free(root);                     // then the level itself
}

/* bookFree		    frees everything in the book (used at program exit)
Parameters:
struct Book *book	the book to tear down

Returns:
nothing (void) */
void bookFree(struct Book *book) {
    treeFreeAll(book->bids);
    treeFreeAll(book->asks);
    book->bids = NULL;
    book->asks = NULL;
}

/* Invariant checking (used by the test harness)

levelCheckInvariants	    verifies BST ordering + AVL balance for a subtree

Parameters:
struct PriceLevel *root		root of the subtree to check
long low			        every price must be > low
long high			        every price must be < high

Returns:
int	the true height of the subtree if all invariants hold,
or -1 if any invariant is violated

Notes:
The (low, high) window is for checking BST ordering: each node must 
sit inside the window, and the window tightens as we descend.  
Also need to recompute heights from scratch and compare against the 
stored height fields, and check that every balance factor is within [-1, +1]. */
int levelCheckInvariants(struct PriceLevel *root, long low, long high) {
    if (root == NULL) {
        return (0); // an empty tree is valid, height 0
    }
    // BST ordering check
    if ((root->price <= low) || (root->price >= high)) {
        printf("INVARIANT FAIL: price %ld out of BST order\n", root->price);
        return (-1);
    }
    // recurse, tightening the window on each side
    int leftH = levelCheckInvariants(root->left, low, root->price);
    if (leftH < 0) {
        return (-1);
    }
    int rightH = levelCheckInvariants(root->right, root->price, high);
    if (rightH < 0) {
        return (-1);
    }
    // stored height must match the real height
    int realH = 1 + maxInt(leftH, rightH);
    if (root->height != realH) {
        printf("INVARIANT FAIL: node %ld stores height %d, real height %d\n",
               root->price, root->height, realH);
        return (-1);
    }
    // AVL balance check
    int balance = leftH - rightH;
    if ((balance < -1) || (balance > 1)) {
        printf("INVARIANT FAIL: node %ld has balance factor %d\n",
               root->price, balance);
        return (-1);
    }
    return (realH);
}

/* Book display

collectLowest	            in-order traversal that records the first few levels
Parameters:
struct PriceLevel *root		subtree being walked
struct PriceLevel **slots	array to fill with level pointers
int maxCount			    size of the array
int *count			        IN/OUT: how many slots used so far

Returns:
nothing (void)

Notes:
An in-order walk (left, node, right) visits prices in ASCENDING
order, so the first maxCount nodes visited are the LOWEST prices.
The count is passed by pointer (the class pointer trick) so every
recursive call shares one counter and can stop early. */
void collectLowest(struct PriceLevel *root, struct PriceLevel **slots,
                   int maxCount, int *count) {
    if ((root == NULL) || (*count >= maxCount)) {
        return;
    }
    collectLowest(root->left, slots, maxCount, count);
    if (*count < maxCount) {
        slots[*count] = root;
        *count = *count + 1;
    }
    collectLowest(root->right, slots, maxCount, count);
}

/* collectHighest	    reverse in-order walk that records the top levels
Parameters:
(same as collectLowest)

Returns:
nothing (void)

Notes:
Walking right, node, left visits prices in DESCENDING order, so the
first maxCount nodes visited are the HIGHEST prices, exactly the best bids. */
void collectHighest(struct PriceLevel *root, struct PriceLevel **slots,
                    int maxCount, int *count) {
    if ((root == NULL) || (*count >= maxCount)) {
        return;
    }
    collectHighest(root->right, slots, maxCount, count);
    if (*count < maxCount) {
        slots[*count] = root;
        *count = *count + 1;
    }
    collectHighest(root->left, slots, maxCount, count);
}

#define MAX_DISPLAY 10 // most levels ever to show per side

/* bookDisplay		prints the top of the book as a price ladder
Parameters:
struct Book *book	the book to display
int depth		    how many levels to show per side (max 10)

Returns:
nothing (void)

Notes:
Asks are printed highest-first down to the best ask, then bids
from the best bid downward, so the output reads like the ladder
in Figure 1 of my proposal: sellers on top, buyers below. */
void bookDisplay(struct Book *book, int depth) {
    struct PriceLevel *slots[MAX_DISPLAY];
    int count;
    int i;

    if (depth > MAX_DISPLAY) {
        depth = MAX_DISPLAY;
    }

    printf("---------------- BOOK ----------------\n");

    // asks: gather the LOWEST 'depth' ask prices, print them top-down
    count = 0;
    collectLowest(book->asks, slots, depth, &count);
    if (count == 0) {
        printf("  (no asks)\n");
    }
    for (i = count - 1; i >= 0; i--) {      // reverse: highest of the group first
        printf("  ASK  %8ld  x %-6ld\n", slots[i]->price, slots[i]->totalQty);
    }

    printf("  --------- spread ---------\n");

    // bids: gather the HIGHEST 'depth' bid prices, already best-first
    count = 0;
    collectHighest(book->bids, slots, depth, &count);
    if (count == 0) {
        printf("  (no bids)\n");
    }
    for (i = 0; i < count; i++) {
        printf("  BID  %8ld  x %-6ld\n", slots[i]->price, slots[i]->totalQty);
    }
    printf("--------------------------------------\n");
}
