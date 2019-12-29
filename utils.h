/**
 * Print shell prompt
 */
void prompt();

/**
 * Clean up resources before exiting
 * Particularly useful in sighandler
 */
void cleanup();

/** Keep track of arguments */
extern char ** argv;

/** Keep track of current line of input */
extern char * ln;

/** Print shell error */
void serror(char * msg);

/** Print parser error */
void pserror(char * msg);

/** Print internal shell error */
void iserror(char * msg);

// Linked List
#ifndef LINKED_LIST
#define LINKED_LIST
/** 
 * A single node for pid linked list
*/
struct PidNode
{
    int data;
    struct PidNode *next;
};

/** 
 * A pid linked list
*/
struct PidList
{
    struct PidNode *head;
    struct PidNode *end;
    int len;
};

#endif

/** 
 * Append a new node to a linked list
 * @arg l the list to append node to
 * @arg data the number to be added
 * @note This function modifies the list
 * @return the linked list
*/
struct PidList *lappend(struct PidList *l, int data);

/** 
 * Remove a node from the list from the beginning and return the value
 * @warning Since the list is only used for storing pids, then negative numbers are not needed, so -1 is used for indication of error
 * @note This function modifies the list
 * 
 * @arg list linked list
 * @return value of the removed node, or -1 on error or empty list
*/
int ldequeue(struct PidList * list);