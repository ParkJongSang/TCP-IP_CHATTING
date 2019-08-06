#ifndef __USER_H__
#define __USER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USER_SUCCESS 0
#define USER_FAIL -1
#define USER_PN 31

char user[100][20];
int userSize;
int UserHashTable[100][10];

int User_Init_List();
int User_Del_List(char *name);
int User_Add_List(char *name);
int User_Search_List(char *name);
int User_Save_File();
unsigned int User_Get_Key(char *name);

#endif