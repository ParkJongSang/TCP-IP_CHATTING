#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define QUEUE_SUCCESS 0
#define QUEUE_FAIL -1
#define TRUE 1
#define FALSE 0

int MAX_SIZE;

typedef struct Node{
    char name[20];
    long time;
    int type;
    struct Node *next;
    struct Node *prev;
}Node;

typedef struct Queue{
    Node *head;
    Node *tail;
    int queueSize;
}Queue;

Node FreeNode[100];
Queue *FreeQ;
Queue *MallocQ;

int Queue_Init(int size);
int Queue_Push_Back(char *srcName, int type, long time);
int Queue_Pop_Front();
int Queue_Is_Empty(Queue *queue);
int Queue_Is_Full(Queue *queue);
Node *Queue_front();
void Queue_Free();

int Push_Back(Queue *queue, Node *toAdd);
Node *Pop_Front(Queue *queue);

void printQ();

#endif