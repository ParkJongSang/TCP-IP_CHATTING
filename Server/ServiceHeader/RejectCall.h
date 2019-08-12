/* Source IP : One or Many Dest IP */
#ifndef __REJECT_CALL_H__
#define __REJECT_CALL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REJECT_CALL_SUCCESS 0
#define REJECT_CALL_FAIL -1
#define REJECT_CALL_PN 13 /* PRIME NUMBER */

typedef struct RejectCall
{
    /* data */
    char srcName[20];
    char dstName[100][20];
    int dstNameSize;
} RejectCall_t;

RejectCall_t rejectCall[100];
int rejectCallSize;
int rejectCallHashTable[100][10];

int RejectCall_Init_List();
int RejectCall_Del_List(char *key, char *value);
int RejectCall_Add_List(char *key, char *value);
int RejectCall_Search_Key_List(char *name);
int RejectCall_Search_Value_List(char *key, char *value);
int RejectCall_Save_File();
unsigned int RejectCall_Get_Key(char *name);

void printReject();

#endif