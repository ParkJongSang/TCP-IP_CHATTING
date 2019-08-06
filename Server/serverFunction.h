#ifndef __SERVER_FUNCTION_H__
#define __SERVER_FUNCTION_H__

#include "./../Packet.h"
#include "./../Includes.h"
#include "./../ReadNWrite.h"
#include "./../Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SERVER_SUCCESS  0
#define SERVER_FAIL     -1
#define SERVER_MAX_CLIENT_LIST_SIZE 10

typedef struct connectedClient{
    char name[20];
    int fd;
    long time;
    int chatRoom;
    int isPing;
}connClient_t;

connClient_t clientList[SERVER_MAX_CLIENT_LIST_SIZE];
int clientListSize;
int chatRoomList[6][11];
int chatRoomAck[6];
int IS_SIG;

void ServerInit(int qSize);
int Server_Ack_To_Client(int sockFd, char *client, int state);
int Server_Ping_Request(int sockFd, char *client);
int Server_Sign_Up_Ack(int sockFd, char *client);
int Server_Chat_Request(int sockFd, char* client, char *message);
int Server_Chat_Ack(int sockFd, char* client, char *dstClient);
int Server_Exit_Request(int sockFd, char *client);
int Server_Exit_Ack(int sockFd, char *client);
int Server_Create_Chat_Room_Ack(int sockFd, char *client);
int Server_Invite_Client_Request(int sockFd, char *client_1, char *client_2);
int Server_Invite_Client_Ack(int sockFd, char *client_1, char *client_2, char *msg);
int Server_Exit_Chat_Romm_Ack(int sockFd, char *client);
int Server_Alram_Request(int sockFd, char *msg);

int Server_Add_Lettering_Ack(int sockFd, char *client);
int Server_Del_Lettering_Ack(int sockFd, char *client);
int Server_Add_Reject_Call_Ack(int sockFd, char *client);
int Server_Del_Reject_Call_Ack(int sockFd, char *client);
int Server_Add_Call_Forwarding_Ack(int sockFd, char *client);
int Server_Del_Call_Forwarding_Ack(int sockFd, char *client);

int Server_Search_Client(char* client);
int Server_Add_Client_List(int sockFd, char *client, long time);
int Server_Del_Client_List(int sockFd, char *client);
int Server_Check_Packet_Time_Over(long curTime);
int Server_Check_Client_Time_Over(long curTime);

void Server_Siging_Handler(int signo);

#endif