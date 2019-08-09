#ifndef __CLIENT_FUNCTION_H__
#define __CLIENT_FUNCTION_H__

#include "./../Packet.h"
#include "./../Includes.h"
#include "./../ReadNWrite.h"
#include "./../Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLIENT_SUCCESS  0
#define CLIENT_FAIL     -1

int IS_SIG;

void ClientInit(int qSize);
int Client_Connect_To_Server(int sockFd, char *self);
int Client_Ping_Ack(int sockFd, char *self);
int Client_Sign_Up(int sockFd, char *self);
int Client_Chat_Request(int sockFd, char* self, char *message);
int Client_Chat_Ack(int sockFd, char* self, char *dstClient);
int Client_Exit_Request(int sockFd, char* self);
int Client_Exit_Ack(int sockFd, char* self);
int Client_Create_ChatRoom(int sockFd, char* self);
int Client_Invite_Client_Request(int sockFd, char *self, char *another);
int Client_Invite_Client_Ack(int sockFd, char *self, char *another, char *msg);
int Client_Exit_Chat_Room_Request(int sockFd, char *self);
int Client_Exit_Chat_Room_Ack(int sockFd, char *self);
int Client_Alram_Ack(int sockFd, char* self);
int Client_All_Ueser_Request(int sockFd, char *self);

int Client_Add_Lettering(int sockFd, char *self, char *name);
int Client_Del_Lettering(int sockFd, char *self);
int Client_Add_Reject_Call(int sockFd, char *self, char *name);
int Client_Del_Reject_Call(int sockFd, char *self, char *name);
int Client_Add_Call_Forwarding(int sockFd, char *self, char *name);
int Client_Del_Call_Forwarding(int sockFd, char *self);

int Client_Check_Packet_Time_Over(int sockFd, char *self, long curTime);
void Client_Sigint_Handler(int signo);

#endif