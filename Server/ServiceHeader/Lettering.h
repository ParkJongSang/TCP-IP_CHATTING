/* IP_Address : Name */
#ifndef __LETTERING_H__
#define __LETTERING_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LETTERING_SUCCESS 0
#define LETTERING_FAIL -1
#define LETTERINT_PN 29  /* PRIME NUMBER */

typedef struct Lettering
{
    /* data */
    char srcName[20];
    char dstName[20];
} Lettering_t;

Lettering_t lettering[100];
int letteringSize;
int letteringHashTable[100][10];

int Lettering_Init_List();
int Lettering_Del_List(char *name);
int Lettering_Add_List(char *key, char* value);
int Lettering_Search_List(char *name);
int Lettering_Save_File();
unsigned int Lettering_Get_Key(char *name);

#endif