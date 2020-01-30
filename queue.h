#ifndef HUFFMAN_QUEUE_H
#define HUFFMAN_QUEUE_H

#include <stdlib.h>

typedef struct tree
{
    unsigned char value;
    int priority;
    struct tree *left;
    struct tree *right;
} tree;

typedef struct queue
{
    tree *value;
    struct queue *next;
} queue;

queue *pop(queue *temp, tree **result);

queue *push(queue *temp, tree *val);

#endif //HUFFMAN_QUEUE_H
