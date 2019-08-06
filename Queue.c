#include "Queue.h"

int Queue_Init(int size){
    int i = 0;
    MAX_SIZE = size;
    if((FreeQ = malloc(sizeof(Queue))) == NULL){
        return QUEUE_FAIL;
    }
    if((MallocQ = malloc(sizeof(Queue))) == NULL){
        return QUEUE_FAIL;
    }

    for(i = 0; i < MAX_SIZE; i++){
        memset(FreeNode[i].name, 0x00, sizeof(FreeNode[i].name));
        FreeNode[i].next = NULL;
        FreeNode[i].prev = NULL;
        FreeNode[i].time = -1;
        FreeNode[i].type = -1;
        Push_Back(FreeQ, &FreeNode[i]);
    }

    return QUEUE_SUCCESS;
}


int Queue_Push_Back(char *srcName, int type, long time){

    Node *popFreeQ = NULL;

    if((popFreeQ = Pop_Front(FreeQ)) == NULL){
        return QUEUE_FAIL;
    }

    strcpy(popFreeQ -> name, srcName);
    popFreeQ -> type = type;
    popFreeQ -> time = time;

    Push_Back(MallocQ, popFreeQ);

    return QUEUE_SUCCESS;
}

int Queue_Pop_Front(){
    Node *popMallocQ = NULL;
    
    if((popMallocQ = Pop_Front(MallocQ)) == NULL){
        return QUEUE_FAIL;
    }

    if(Push_Back(FreeQ, popMallocQ) == -1){
        return QUEUE_FAIL;
    }
    printf("pop!!!\n");

    return popMallocQ -> type;
}

int Queue_Is_Empty(Queue *queue)
{
    if(queue -> queueSize <= 0){
        return TRUE;
    }
    return FALSE;
}

int Queue_Is_Full(Queue *queue){
    if(queue -> queueSize >= MAX_SIZE){
        return TRUE;
    }
    return FALSE;
}

void Queue_Free(){
    if(FreeQ != NULL){
        free(FreeQ);
        FreeQ = NULL;
    }
    if(MallocQ != NULL){
        free(MallocQ);
        MallocQ = NULL;
    }
}

int Push_Back(Queue *queue, Node *toAdd){
    if(queue == NULL ||  toAdd == NULL || Queue_Is_Full(queue) == TRUE){
        return QUEUE_FAIL;
    }

    if(Queue_Is_Empty(queue) == TRUE){
        queue -> head = toAdd;
        queue -> tail = toAdd;
    }else{
        queue -> tail -> next = toAdd;
        toAdd -> prev = queue -> tail;
        queue -> tail = toAdd;
    }

    queue -> queueSize += 1;

    return QUEUE_SUCCESS;
}

Node *Queue_front(){
    if(MallocQ == NULL || FreeQ == NULL){
        return NULL;
    }
    return MallocQ -> head;
}

Node *Pop_Front(Queue *queue){
    if(queue == NULL || Queue_Is_Empty(queue) == TRUE){
        return NULL;
    }
    if(queue -> head == NULL){
        return NULL;
    }
    Node *toDel = queue -> head;

    queue -> head = toDel -> next;

    toDel -> next = NULL;
    toDel -> prev = NULL;

    queue -> queueSize -= 1;
    return toDel;
}

void printQ(){
    Node *freeHead;
    freeHead = FreeQ -> head;

    Node *mallocHead;
    mallocHead = MallocQ -> head;

    while(freeHead != NULL){
        printf("%d ", freeHead ->type);
        freeHead = freeHead -> next;
    }
    printf("\n");
    while(mallocHead != NULL){
        printf("%d ", mallocHead ->type);
        mallocHead = mallocHead -> next;
    }
    printf("\n");
}