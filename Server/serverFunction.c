#include "serverFunction.h"

void ServerInit(int qSize)
{
    Queue_Init(qSize);
    clientListSize = 0;
}

int Server_Create_Chat_Room_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PAKCET_TYPE_CREATE_CHATROOM_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Ack_To_Client(int sockFd, char *client, int state)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    char stateStr[5];
    strcpy(stateStr, "NO");

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_CONN_ACK);

    if (state == 1)
    {
        strcpy(stateStr, "YES");
    }

    head.bodyLength = htonl(strlen(stateStr));
    strcpy(body.str, stateStr);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(stateStr))) != strlen(stateStr))
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Ping_Request(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.dstName, client);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_PING_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    Queue_Push_Back(client, PACKET_TYPE_PING_ACK, curTime);

    return SERVER_SUCCESS;
}

int Server_Sign_Up_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_SIGN_UP_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Chat_Request(int sockFd, char *client, char *message)
{
    if (client == NULL || strlen(client) == 0 || message == NULL || strlen(message) == 0)
    {
        return SERVER_FAIL;
    }
    int clientIdx;
    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, client);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_STR_REQ);
    head.bodyLength = htonl(strlen(message));

    strcpy(body.str, message);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(message))) != strlen(message))
    {
        return SERVER_FAIL;
    }

    Queue_Push_Back(client, PACKET_TYPE_STR_ACK, curTime);

    return SERVER_SUCCESS;
}

int Server_Chat_Ack(int sockFd, char *client, char *dstClient)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.dstName, client);
    strcpy(head.dstName, dstClient);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_STR_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Exit_Request(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_EXIT_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    Queue_Push_Back(client, PACKET_TYPE_EXIT_ACK, curTime);

    return SERVER_SUCCESS;
}

int Server_Exit_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_EXIT_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Invite_Client_Request(int sockFd, char *client_1, char *client_2)
{
    if (client_1 == NULL || strlen(client_1) == 0 || client_2 == NULL || strlen(client_2) == 0)
    {
        return SERVER_FAIL;
    }

    long curTime = time(NULL);
    int client_1_Idx;
    int client_2_Idx;
    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    if ((client_1_Idx = Server_Search_Client(client_1)) < 0)
    {
        return SERVER_FAIL;
    }

    if ((client_2_Idx = Server_Search_Client(client_2)) < 0 && clientList[client_2_Idx].chatRoom > 0)
    {
        return SERVER_FAIL;
    }

    strcpy(head.srcName, client_1);
    strcpy(head.dstName, client_2);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_INVITE_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    Queue_Push_Back(client_2, PACKET_TYPE_INVITE_ACK, curTime);

    return SERVER_SUCCESS;
}

int Server_Invite_Client_Ack(int sockFd, char *client_1, char *client_2, char *msg)
{
    if (client_1 == NULL || strlen(client_1) == 0 || client_2 == NULL || strlen(client_2) == 0)
    {
        return SERVER_FAIL;
    }

    int client_1_Idx;
    int client_2_Idx;
    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    if ((client_1_Idx = Server_Search_Client(client_1)) < 0)
    {
        return SERVER_FAIL;
    }

    if ((client_2_Idx = Server_Search_Client(client_1)) < 0 && clientList[client_2_Idx].chatRoom > 0)
    {
        return SERVER_FAIL;
    }

    strcpy(head.srcName, client_1);
    strcpy(head.dstName, client_2);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_INVITE_ACK);
    head.bodyLength = htonl(strlen(msg));
    strcpy(body.str, msg);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(msg))) != strlen(msg))
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Exit_Chat_Romm_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.dstName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_EXIT_CHATROOM_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Alram_Request(int sockFd, char *msg)
{
    if (msg == NULL || strlen(msg) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_ALRAM_REQ);
    head.bodyLength = htonl(strlen(msg));

    strcpy(body.str, msg);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(msg))) != strlen(msg))
    {
        return SERVER_FAIL;
    }
    Queue_Push_Back("server", PACKET_TYPE_ALRAM_ACK, curTime);

    return SERVER_SUCCESS;
}

int Server_Add_Lettering_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_LETTERING_ADD_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}
int Server_Del_Lettering_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_LETTERING_DEL_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}
int Server_Add_Reject_Call_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_REJECT_CALL_ADD_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}
int Server_Del_Reject_Call_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_REJECT_CALL_DEL_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }
}
int Server_Add_Call_Forwarding_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_CALL_FORWARDING_ADD_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}
int Server_Del_Call_Forwarding_Ack(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, client);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_CALL_FORWARDING_DEL_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return SERVER_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return SERVER_FAIL;
    }

    return SERVER_SUCCESS;
}

int Server_Search_Client(char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }
    int i = 0;
    for (i = 0; i < clientListSize; i++)
    {
        if (strcmp(clientList[i].name, client) == 0)
        {
            return i;
        }
    }
    return SERVER_FAIL;
}

int Server_Add_Client_List(int sockFd, char *client, long time)
{
    if (client == NULL || strlen(client) == 0)
    {
        return SERVER_FAIL;
    }
    if (strcmp(client, "unknown") != 0 && Server_Search_Client(client) >= 0)
    {
        return SERVER_FAIL;
    }
    clientList[clientListSize].fd = sockFd;
    strcpy(clientList[clientListSize].name, client);
    clientList[clientListSize].chatRoom = 0;
    clientList[clientListSize].time = time;
    clientListSize += 1;

    return SERVER_SUCCESS;
}

int Server_Del_Client_List(int sockFd, char *client)
{
    if (client == NULL || strlen(client) == 0)
    {
        printf("Fail Argument.\n");
        return SERVER_FAIL;
    }
    int clientIdx;
    int i;
    if ((clientIdx = Server_Search_Client(client)) < 0)
    {
        printf("Search Fail...\n");
        return SERVER_FAIL;
    }

    close(clientList[clientIdx].fd);

    for (i = clientIdx; i < clientListSize; i++)
    {
        clientList[i].chatRoom = clientList[i + 1].chatRoom;
        clientList[i].fd = clientList[i + 1].fd;
        memset(clientList[i].name, 0x00, sizeof(clientList[i].name));
        strcpy(clientList[i].name, clientList[i + 1].name);
        clientList[i].time = clientList[i + 1].time;
    }

    memset(clientList[clientListSize - 1].name, 0x00, sizeof(clientList[clientListSize - 1].name));
    clientList[clientListSize - 1].fd = -1;
    clientList[clientListSize - 1].chatRoom = 0;
    clientList[clientListSize - 1].time = -1;
    clientListSize -= 1;

    printf("Del Client.\n");

    return SERVER_SUCCESS;
}

int Server_Check_Packet_Time_Over(long curTime){
    Node *cursor = NULL;
    Node *temp = NULL;
    cursor = MallocQ -> head;
    int ackType = -1;
    while(cursor != NULL){
        if(curTime - (cursor -> time) > 5){
            temp = Queue_front();
            if(temp -> type == PACKET_TYPE_PING_ACK){
                int idx = Server_Search_Client(temp -> name);
                if(idx < 0){
                    printf("Cannot Search.\n");
                    break;
                }
                if(Server_Exit_Request(clientList[idx].fd, clientList[idx].name) == SERVER_FAIL){
                    printf("Exit Req Fail.\n");
                }
                printf("EXIT CLIENT.\n");
            }
            printf("[%d]TimeOut PACKET OUT.\n", temp -> type);
            Queue_Pop_Front();
        }else{
            cursor = cursor -> next;
        }
    }

}

int Server_Check_Client_Time_Over(long curTime){
    int i = 0;
    int ret = 0;
    for(i = 0; i < clientListSize; i++){
        if(clientList[i].isPing == 0 && curTime - clientList[i].time > 3){
            if(Server_Ping_Request(clientList[i].fd, clientList[i].name) == SERVER_FAIL){
                return SERVER_FAIL;
            }
            clientList[i].isPing = 1;
            printf("Send Ping.\n");
        }
    }
    return SERVER_SUCCESS;
}


void Server_Siging_Handler(int signo)
{
    IS_SIG = 1;
}