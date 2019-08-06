#include "clientFunction.h"

void ClientInit(int qSize)
{
    Queue_Init(qSize);
}

int Client_Connect_To_Server(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_CONN_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }
    Queue_Push_Back(self, PACKET_TYPE_CONN_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Ping_Ack(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, self);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_PING_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }

    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    return CLIENT_SUCCESS;
}

int Client_Sign_Up(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_SIGN_UP_REQ);
    head.bodyLength = htonl(strlen(self));
    strcpy(body.str, self);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(self))) != strlen(self))
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_SIGN_UP_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Chat_Request(int sockFd, char *self, char *message)
{
    if (self == NULL || strlen(self) == 0 || message == NULL || strlen(message) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_STR_REQ);
    head.bodyLength = htonl(strlen(message));

    strcpy(body.str, message);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(message))) != strlen(message))
    {
        return CLIENT_FAIL;
    }
    
    Queue_Push_Back(self, PACKET_TYPE_STR_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Chat_Ack(int sockFd, char *self, char *dstClient)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, self);
    strcpy(head.dstName, dstClient);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_STR_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    return CLIENT_SUCCESS;
}

int Client_Exit_Request(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    long curTime = time(NULL);
    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_EXIT_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_EXIT_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Create_ChatRoom(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    long curTime = time(NULL);
    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_CREATE_CHATROOM_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_EXIT_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Exit_Ack(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, self);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_EXIT_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    return CLIENT_SUCCESS;
}

int Client_Invite_Client_Request(int sockFd, char *self, char *another)
{
    if (self == NULL || another == NULL || strlen(self) == 0 || strlen(another) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    strcpy(head.dstName, another);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_INVITE_REQ);

    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_INVITE_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Invite_Client_Ack(int sockFd, char *self, char *another, char *msg)
{
    if (self == NULL || another == NULL || strlen(self) == 0 || strlen(another) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, self);
    strcpy(head.dstName, another);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_INVITE_ACK);
    head.bodyLength = htonl(strlen(msg));
    strcpy(body.str, msg);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(msg))) != strlen(msg))
    {
        return CLIENT_FAIL;
    }

    return CLIENT_SUCCESS;
}

int Client_Exit_Chat_Room_Request(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_EXIT_CHATROOM_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_EXIT_CHATROOM_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Alram_Ack(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;

    strcpy(head.srcName, self);
    head.time = htonl(time(NULL));
    head.type = htonl(PACKET_TYPE_ALRAM_ACK);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    return CLIENT_SUCCESS;
}

int Client_Add_Lettering(int sockFd, char *self, char *name)
{
    if (self == NULL || strlen(self) == 0 || name == NULL || strlen(name) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_LETTERING_ADD_REQ);
    head.bodyLength = htonl(strlen(name));
    strcpy(body.str, name);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(name))) != strlen(name))
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_LETTERING_ADD_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Del_Lettering(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_LETTERING_DEL_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_LETTERING_DEL_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Add_Reject_Call(int sockFd, char *self, char *name)
{
    if (self == NULL || strlen(self) == 0 || name == NULL || strlen(name) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_REJECT_CALL_ADD_REQ);
    head.bodyLength = htonl(strlen(name));

    strcpy(body.str, name);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(name))) != strlen(name))
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_REJECT_CALL_ADD_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Del_Reject_Call(int sockFd, char *self, char *name)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_REJECT_CALL_DEL_REQ);
    head.bodyLength = htonl(strlen(name));

    strcpy(body.str, name);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(name))) != strlen(name))
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_REJECT_CALL_DEL_ACK, curTime);

    return CLIENT_SUCCESS;
}
int Client_Add_Call_Forwarding(int sockFd, char *self, char *name)
{
    if (self == NULL || strlen(self) == 0 || name == NULL || strlen(name) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_CALL_FORWARDING_ADD_REQ);
    head.bodyLength = htonl(strlen(name));

    strcpy(body.str, name);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, strlen(name))) != strlen(name))
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_REJECT_CALL_ADD_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Del_Call_Forwarding(int sockFd, char *self)
{
    if (self == NULL || strlen(self) == 0)
    {
        return CLIENT_FAIL;
    }

    size_t writeByte;
    packetHead_t head;
    packetBody_t body;
    long curTime = time(NULL);

    strcpy(head.srcName, self);
    head.time = htonl(curTime);
    head.type = htonl(PACKET_TYPE_CALL_FORWARDING_DEL_REQ);
    head.bodyLength = htonl(0);

    if ((writeByte = writePacket(sockFd, &head, sizeof(head))) != sizeof(head))
    {
        return CLIENT_FAIL;
    }
    if ((writeByte = writePacket(sockFd, &body, 0)) != 0)
    {
        return CLIENT_FAIL;
    }

    Queue_Push_Back(self, PACKET_TYPE_CALL_FORWARDING_DEL_ACK, curTime);

    return CLIENT_SUCCESS;
}

int Client_Check_Packet_Time_Over(int sockFd, char *self, long curTime)
{
    if (self == NULL || strlen(self) == 0 || Queue_Is_Empty(MallocQ) == TRUE)
    {
        return CLIENT_FAIL;
    }
    Node *cursor = NULL;
    Node *temp = NULL;
    cursor = MallocQ->head;
    int ackType = -1;
    while (cursor != NULL)
    {
        if (curTime - (cursor->time) > 5)
        {
            return Queue_Pop_Front();
        }
        else
        {
            cursor = cursor->next;
        }
    }

    return CLIENT_SUCCESS;
}

void Client_Sigint_Handler(int signo)
{
    IS_SIG = 1;
}