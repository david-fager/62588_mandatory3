#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList {
    // doubly-linked list
    struct memoryList *last;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz) {
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */

    // If the linked list has any nodes, they'll be freed
    struct memoryList *temp = head;
    struct memoryList *executioner;
    while (temp != NULL) {
        executioner = temp;
        temp = temp->next;
        myfree(executioner);
    }
    free(head);

    myMemory = malloc(sz);

    /* TODO: Initialize memory management structure. */

    // The head is initialised to the first node, which first contains the whole memory
    head = malloc(sizeof(struct memoryList));
    head->alloc = 0;
    head->size = mySize;
    head->ptr = myMemory;
    head->last = NULL;
    head->next = NULL;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested) {
    assert((int) myStrategy > 0);

    switch (myStrategy) {
        case NotSet:
            return NULL;
        case First:
            return NULL;
        case Best:
            return NULL;
        case Worst:;

            struct memoryList *trav = head;
            struct memoryList *worst = NULL;

            // Finds the 'worst' link by looping through the linked list
            while (trav != NULL) {

                if (trav->next == NULL) {
                    // If at the end of the list

                    if (worst != NULL && trav->size > worst->size) {
                        // if the last node is bigger than the biggest found 'worst' node

                        worst = trav;
                    } else if (worst == NULL && trav->size < requested) {
                        // if no suitable node has been found and the last node is too small, then return null

                        return; // the loop stops running
                    }
                }

                if (trav->size < requested || trav->alloc == 1) {
                    // If either the requested size is too big or the node already is 'allocated'
                    trav = trav->next;
                    continue; // skips the rest of this loop iteration and continues with next loop iteration
                }

                if (worst == NULL || trav->size > worst->size) {
                    // Sets 'worst' to the node with the biggest size
                    worst = trav;
                }

                trav = trav->next;
            }

            // The rest of this function allocates the requested memory
            if (worst->size - requested != 0) {
                struct memoryList *temp = malloc(sizeof(struct memoryList));

                // Exchanging pointers
                temp->last = worst; // new node points backwards, to worst (its previous node)
                if (worst->next != NULL) {
                    // If the linked list continues, otherwise worst->next is NULL
                    temp->next = worst->next; // new node points to next node in linked list
                    worst->next->last = temp; // next node in linked list gets its previous (last) pointer set to new node
                }
                worst->next = temp; // previous node points to new node
                // At this point, all pointers of the nodes before temp was put in, are now pointing to the new node (temp)
                // Meaning that temp is now fully integrated, where ever it was put in (in the middle or at the end of the list)

                // Sets the rest of the link's variables
                temp->size = worst->size - requested; // Giving new node the non-allocated size

                temp->ptr = worst->ptr + requested; // New node pointer points to actual memory space (myMemory)
                temp->alloc = 0; // New node now holds the remainder of the memory, and it is free
            }

            worst->size = requested; // The memory that is allocated now has a new size that is the requested size
            worst->alloc = 1; // The worst suitable memory now allocated and pointers are set to new node

            return worst->ptr;
        case Next:
            return NULL;
    }

    return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void *block) {

    struct memoryList *trav = head;

    while (trav != NULL) {

        if (trav->ptr == block) {
            // Found the memory block that was given

            trav->alloc = 0; // Alloc is zero on for the node

            if (trav->last != NULL && trav->last->alloc == 0) {
                // If the block to be freed has a previous block that is not allocated, merge the two blocks
                trav->last->size += trav->size; // adds the freed blocks memory size to the previous blocks size

                if (trav->next != NULL) {
                    // Exchanging pointers
                    trav->last->next = trav->next;
                    trav->next->last = trav->last;
                }

                if (trav->next != NULL && trav->next->alloc == 0) {
                    // Checks if the right should also be merged
                    myfree(trav->next);
                }

                // Clearing out the now empty node's variables
                trav->size = 0;
                trav->ptr = NULL;
                trav->last = NULL;
                trav->next = NULL;

                free(trav); // Frees the linked list struct memory attached to the node
                break;
            } else if (trav->next != NULL && trav->next->alloc == 0) {
                // If the node on the left could not be merged, then this merges with the right one if condition is met
                myfree(trav->next);
            }

            break;
        }

        trav = trav->next;
    }

}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes() {
    // Amount is incremented for each node that has alloc == 0
    int amount = 0;
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->alloc == 0) {
            if (temp->size != 0) {
                amount++;
            }
        }
        temp = temp->next;
    }
    print_memory();
    return amount;
}

/* Get the number of bytes allocated */
int mem_allocated() {
    // Amount is incremented with the number of bytes allocated for each node that has alloc == 1
    int amount = 0;
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->alloc == 1) {
            amount += temp->size;
        }
        temp = temp->next;
    }
    return amount;
}

/* Number of non-allocated bytes */
int mem_free() {
    // Amount is incremented with the number of bytes allocated for each node that has alloc == 1
    int amount = 0;
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->alloc == 0) {
            amount += temp->size;
        }
        temp = temp->next;
    }
    return amount;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free() {
    // Loops through the list and as long as alloc == 0 then adds to amount,
    // and if alloc == 1 then save amount if larger than what largest holds
    int largest = 0;
    int amount = 0;
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->alloc == 0) {
            amount += temp->size;
        } else {
            if (amount > largest) {
                largest = amount;
            }
            amount = 0;
        }
        temp = temp->next;
    }
    if (amount > largest) {
        largest = amount;
    }
    return largest;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size) {
    // increments amount for each node that has a smaller size than the size argument
    int amount = 0;
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->alloc == 0) {
            if (temp->size < size) {
                amount++;
            }
        }
        temp = temp->next;
    }
    return amount;
}

char mem_is_alloc(void *ptr) {
    // Looks for the pointer in all the nodes pointers, and if found returns the nodes alloc value
    struct memoryList *temp = head;
    while (temp != NULL) {
        if (temp->ptr == ptr) {
            return temp->alloc;
        }
        temp = temp->next;
    }
    return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool() {
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total() {
    return mySize;
}

// Get string name for a strategy. 
char *strategy_name(strategies strategy) {
    switch (strategy) {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char *strategy) {
    if (!strcmp(strategy, "best")) {
        return Best;
    } else if (!strcmp(strategy, "worst")) {
        return Worst;
    } else if (!strcmp(strategy, "first")) {
        return First;
    } else if (!strcmp(strategy, "next")) {
        return Next;
    } else {
        return 0;
    }
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory() {
    /*
    int number = 0;
    struct memoryList* trav = head;
    while (trav != NULL) {
        printf("Node [%d] has memory size: %d, and allocation bool: %d\n", number++, trav->size, trav->alloc);
        trav = trav->next;
    }
    */
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */
void print_memory_status() {
    printf("%d out of %d bytes allocated.\n", mem_allocated(), mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n", mem_free(), mem_holes(),
           mem_largest_free());
    printf("Average hole size is %f.\n\n", ((float) mem_free()) / mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if (argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat, 500);

    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();

}
