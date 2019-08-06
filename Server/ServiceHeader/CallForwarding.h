/* Source IP : Dest IP */
#ifndef __CALL_FORWORDING_H__
#define __CALL_FORWORDING_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CALL_FORWARDING_SUCCESS 0
#define CALL_FORWARDING_FAIL -1
#define CALL_FORWARDING_PN 23  /* PRIME NUMBER */

typedef struct CallForwarding
{
    /* data */
    char srcName[20];
    char dstName[20];
} CallForwarding_t;

CallForwarding_t callForwarding[100];
int callForwardingSize;
/* Every HashTable's 0 Index is Chain Size */
int callForwardingHashTable[100][10];

int CallForwarding_Init_List();
int CallForwarding_Del_List(char *name);
int CallForwarding_Add_List(char *key, char *value);
int CallForwarding_Search_List(char *name);
int CallForwarding_Save_File();
unsigned int CallForwarding_Get_Key(char *name);

#endif