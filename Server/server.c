#include "./../Includes.h"
#include "./ServiceHeader/CallForwarding.h"
#include "./ServiceHeader/Lettering.h"
#include "./ServiceHeader/RejectCall.h"
#include "./ServiceHeader/User.h"
#include "serverFunction.h"

int main(void)
{
    int curClientCnt = 0;
    int connectFD, listenFD;
    bool reuseFlag = true;
    struct timeval timeout;
    fd_set reads;

    packetBody_t body;
    packetHead_t head;

    struct sockaddr_in listenSocket, connectSocket;

    listenFD = socket(PF_INET, SOCK_STREAM, 0);
    IS_SIG = 0;
    if (User_Init_List() == USER_FAIL)
    {
        printf("USER INIT FAIL.\n");
        return -1;
    }
    if (Lettering_Init_List() == LETTERING_FAIL)
    {
        printf("LETTERING INIT FAIL.\n");
        return -1;
    }
    if (RejectCall_Init_List() == REJECT_CALL_FAIL)
    {
        printf("REJECTCALL INIT FAIL.\n");
        return -1;
    }
    if (CallForwarding_Init_List() == CALL_FORWARDING_FAIL)
    {
        printf("CALLFORWARING INIT FAIL.\n");
        return -1;
    }
    signal(SIGINT, Server_Siging_Handler);
    memset(&listenSocket, 0x00, sizeof(listenSocket));
    memset(&body, 0x00, sizeof(body));
    memset(&head, 0x00, sizeof(head));

    listenSocket.sin_family = AF_INET;
    listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
    listenSocket.sin_port = htons(10000);

    ServerInit(100);
    setsockopt(listenFD, SOL_SOCKET, SO_REUSEPORT, (char *)&reuseFlag, sizeof(reuseFlag));
    fcntl(listenFD, F_SETFL, O_NONBLOCK);

    if (bind(listenFD, (struct sockaddr *)&listenSocket, sizeof(listenSocket)) == -1)
    {
        perror("[BIND]");
        return -1;
    }

    if (listen(listenFD, 5) == -1)
    {
        perror("[LISTEN]");
        return -1;
    }

    while (1)
    {

        int fdNum = 0;
        int i = 0;
        long curTime = time(NULL);
        int fdMax = listenFD;

        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;

        if (IS_SIG == 1)
        {
            for (i = 0; i < clientListSize; i++)
            {
                Server_Exit_Request(clientList[i].fd, clientList[i].name);
            }
            break;
        }

        FD_ZERO(&reads);
        FD_SET(listenFD, &reads);

        for (i = 0; i < clientListSize; i++)
        {
            if (fdMax < clientList[i].fd)
            {
                fdMax = clientList[i].fd;
            }
            FD_SET(clientList[i].fd, &reads);
        }
        if ((fdNum = select(fdMax + 1, &reads, 0, 0, &timeout)) == -1)
        {
            perror("[SELECT]");
            if (IS_SIG == 1)
            {
                for (i = 0; i < clientListSize; i++)
                {
                    Server_Exit_Request(clientList[i].fd, clientList[i].name);
                }
                break;
            }
            break;
        }
        if (fdNum == 0)
        {
            /* Add time on Every Wait Queue Value */
            Server_Check_Client_Time_Over(time(NULL));
            Server_Check_Packet_Time_Over(time(NULL));
            if (IS_SIG == 1)
            {
                for (i = 0; i < clientListSize; i++)
                {
                    Server_Exit_Request(clientList[i].fd, clientList[i].name);
                }
                break;
            }
            continue;
        }

        if (FD_ISSET(listenFD, &reads))
        {
            if(curClientCnt >= 10){
                printf("MAX CLIENT.\n");
                continue;
            }
            socklen_t addrSize = sizeof(connectSocket);
            while ((connectFD = accept(listenFD, (struct sockaddr *)&connectSocket, &addrSize)) < 0)
            {
                sleep(0.5);
            }
            if (Server_Add_Client_List(connectFD, "unknown", time(NULL)) == -1)
            {
                printf("add Fail.\n");
            }
            if (fcntl(connectFD, F_SETFL, O_NONBLOCK) == -1)
            {
                printf("FCNTL ERROR.\n");
            }
            printf("[CONNECTED] : UNKNOWN CLIENT\n");
            curClientCnt += 1;
            fdNum -= 1;
        }

        while (fdNum > 0)
        {
            for (i = 0; i < clientListSize; i++)
            {
                if (FD_ISSET(clientList[i].fd, &reads))
                {
                    size_t readBytes;

                    char client[20];
                    char client_lettering[20];
                    char client_reject[20];
                    char client_forwarding[20];
                    clientList[i].time = time(NULL);
                    memset(&head, 0x00, sizeof(head));
                    memset(&body, 0x00, sizeof(body));
                    if ((readBytes = readPacket(clientList[i].fd, &head, sizeof(head))) != sizeof(head))
                    {
                        fdNum--;
                        continue;
                    }
                    if ((readBytes = readPacket(clientList[i].fd, &body, ntohl(head.bodyLength))) != ntohl(head.bodyLength))
                    {
                        fdNum--;
                        continue;
                    }

                    int msgType = ntohl(head.type);

                    if (msgType == PACKET_TYPE_SIGN_UP_REQ)
                    {
                        printf("send SignUp Ack\n");
                        strcpy(client, body.str);
                        if (User_Add_List(body.str) == USER_FAIL)
                        {
                            printf("USER ADD FAIL\n");
                        }
                        if (User_Save_File() == USER_FAIL)
                        {
                            printf("USER SAVE FAIL\n");
                        }
                        Server_Sign_Up_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_LETTERING_ADD_REQ)
                    {
                        printf("send Lettering Ack\n");
                        strcpy(client, head.srcName);
                        Lettering_Add_List(client, body.str);
                        Lettering_Save_File();
                        Server_Add_Lettering_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_REJECT_CALL_ADD_REQ)
                    {
                        printf("send Reject Ack\n");
                        strcpy(client, head.srcName);
                        RejectCall_Add_List(client, body.str);
                        RejectCall_Save_File();
                        Server_Add_Reject_Call_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_CALL_FORWARDING_ADD_REQ)
                    {
                        printf("send Forwarding Ack\n");
                        strcpy(client, head.srcName);
                        CallForwarding_Add_List(client, body.str);
                        CallForwarding_Save_File();

                        Server_Add_Call_Forwarding_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_CONN_REQ)
                    {
                        printf("send Connect Ack\n");
                        strcpy(client, head.srcName);
                        int state = 0;
                        /* CHECK ID IS VALID OR INVALID */
                        if (Server_Search_Client(client) >= 0)
                        {
                            printf("Aleady Connected.\n");
                        }
                        else if (User_Search_List(client) >= 0)
                        {
                            printf("Matched.\n");
                            memset(clientList[i].name, 0x00, sizeof(clientList[i].name));
                            strcpy(clientList[i].name, client);
                            clientList[i].time = time(NULL);
                            state = 1;
                        }

                        Server_Ack_To_Client(clientList[i].fd, client, state);
                    }
                    else if (msgType == PACKET_TYPE_EXIT_REQ)
                    {
                        printf("send EXIT Ack\n");
                        strcpy(client, head.srcName);
                        Server_Exit_Ack(clientList[i].fd, clientList[i].name);
                        Server_Del_Client_List(clientList[i].fd, clientList[i].name);
                    }
                    else if (msgType == PACKET_TYPE_EXIT_CHATROOM_REQ)
                    {
                        printf("send EXIT CHATROOM Ack\n");

                        int curChatRoom = clientList[i].chatRoom;
                        int curClientInChatRoom = chatRoomList[curChatRoom][0];
                        int j = 0;
                        int searchFlag = 0;
                        char msg[1024];

                        sprintf(msg, "[%s] is Exit Chatroom.", clientList[i].name);

                        strcpy(client, head.srcName);
                        /* Send All Client in Same ChatRooms */
                        Server_Exit_Chat_Romm_Ack(clientList[i].fd, client);

                        for (j = 1; j <= curClientInChatRoom; j++)
                        {
                            if (clientList[i].fd != chatRoomList[curChatRoom][j])
                            {
                                Server_Alram_Request(chatRoomList[curChatRoom][j], msg);
                                printf("Alram Send\n");
                            }
                        }
                        for (j = 1; j <= curClientInChatRoom; j++)
                        {
                            if (clientList[i].fd == chatRoomList[curChatRoom][j])
                            {
                                searchFlag = 1;
                            }
                            if (searchFlag == 1)
                            {
                                chatRoomList[curChatRoom][j] = chatRoomList[curChatRoom][j + 1];
                            }
                        }
                        chatRoomList[curChatRoom][0] -= 1;
                        clientList[i].chatRoom = 0;
                    }
                    else if (msgType == PACKET_TYPE_LETTERING_DEL_REQ)
                    {
                        printf("send Lettering Del Ack\n");
                        strcpy(client, head.srcName);
                        /* Del */
                        Lettering_Del_List(client);
                        Lettering_Save_File();
                        Server_Del_Lettering_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_REJECT_CALL_DEL_REQ)
                    {
                        printf("send RejectCall Del Ack\n");
                        strcpy(client, head.srcName);
                        /* Del */
                        RejectCall_Del_List(client, body.str);
                        RejectCall_Save_File();
                        Server_Del_Reject_Call_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_CALL_FORWARDING_DEL_REQ)
                    {
                        printf("send CallForwarding Del Ack\n");
                        strcpy(client, head.srcName);
                        /* Del */
                        CallForwarding_Del_List(client);
                        CallForwarding_Save_File();
                        Server_Del_Call_Forwarding_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_CREATE_CHATROOM_REQ)
                    {
                        printf("send Create Chatroom Ack\n");
                        strcpy(client, head.srcName);
                        int j = 0;
                        int voidChatRoomIdx = 0;
                        for (j = 1; j < 6; j++)
                        {
                            if (chatRoomList[j][0] == 0)
                            {
                                voidChatRoomIdx = j;
                                break;
                            }
                        }
                        int size = chatRoomList[voidChatRoomIdx][0];
                        chatRoomList[voidChatRoomIdx][size + 1] = clientList[i].fd;
                        chatRoomList[voidChatRoomIdx][0] += 1;
                        clientList[i].chatRoom = voidChatRoomIdx;
                        Server_Create_Chat_Room_Ack(clientList[i].fd, client);
                    }
                    else if (msgType == PACKET_TYPE_INVITE_REQ)
                    {
                        /* INVITE LOGIC */
                        printf("Send Invite\n");
                        int callForwardingIdx = -1;
                        int searchClientIdx = Server_Search_Client(head.dstName);
                        if (searchClientIdx < 0)
                        {
                            printf("Not Find Client.\n");
                            continue;
                        }
                        int isReject = RejectCall_Search_Value_List(head.dstName, head.srcName);
                        if (isReject >= 0)
                        {
                            printf("Rejected Id.\n");
                            Server_Invite_Client_Ack(clientList[i].fd, head.srcName, head.dstName, "no");
                            continue;
                        }
                        int isForward = CallForwarding_Search_List(head.dstName);

                        if (isForward >= 0)
                        {
                            callForwardingIdx = Server_Search_Client(callForwarding[isForward].dstName);
                        }

                        if (callForwardingIdx >= 0)
                        {
                            searchClientIdx = callForwardingIdx;
                            memset(head.dstName, 0x00, sizeof(head.dstName));
                            strcpy(head.dstName, callForwarding[isForward].dstName);
                        }

                        Server_Invite_Client_Request(clientList[searchClientIdx].fd, head.srcName, head.dstName);
                    }
                    else if (msgType == PACKET_TYPE_INVITE_ACK)
                    {
                        /* INVITE LOGIC */
                        if (MallocQ->head->type == msgType)
                        {
                            Queue_Pop_Front();
                        }

                        printf("send Invite Ack.\n");
                        int searchClientIdx = Server_Search_Client(head.srcName);
                        if (searchClientIdx < 0)
                        {
                            printf("Not Find Client.\n");
                            continue;
                        }
                        int beInvitedClientIdx = Server_Search_Client(head.dstName);
                        if (beInvitedClientIdx < 0)
                        {
                            printf("Not Find Client.\n");
                            continue;
                        }

                        if (strcmp(body.str, "yes") == 0)
                        {
                            int curChatRoom = clientList[searchClientIdx].chatRoom;
                            int size = chatRoomList[curChatRoom][0];

                            clientList[beInvitedClientIdx].chatRoom = curChatRoom;
                            chatRoomList[curChatRoom][size + 1] = clientList[beInvitedClientIdx].fd;
                            chatRoomList[curChatRoom][0] += 1;
                        }

                        Server_Invite_Client_Ack(clientList[searchClientIdx].fd, head.srcName, head.dstName, body.str);
                        printf("send Invite Complete.\n");
                    }
                    else if (msgType == PACKET_TYPE_STR_REQ)
                    {
                        int curChatRoom = clientList[i].chatRoom;
                        int curClientInChatRoom = chatRoomList[curChatRoom][0];
                        int j = 0;
                        char msg[1024];
                        int letteringIdx;
                        strcpy(client, head.srcName);
                        if ((letteringIdx = Lettering_Search_List(client)) >= 0)
                        {
                            sprintf(msg, "[%s]", lettering[letteringIdx].dstName);
                        }
                        strcat(msg, " : ");
                        strcat(msg, body.str);
                        for (j = 1; j <= curClientInChatRoom; j++)
                        {
                            if (clientList[i].fd != chatRoomList[curChatRoom][j])
                            {   
                                Server_Chat_Request(chatRoomList[curChatRoom][j], head.srcName, msg);
                            }
                        }
                    }
                    else if (msgType == PACKET_TYPE_STR_ACK)
                    {
                        if(MallocQ->head != NULL && MallocQ->head->type == msgType)
                        {
                            Queue_Pop_Front();
                        }
                        
                        int ackClientIdx = Server_Search_Client(head.dstName);
                        if (ackClientIdx < 0)
                        {
                            printf("Not Find Clinet.\n");
                        }
                        int recvAckClientIdx = Server_Search_Client(head.srcName);
                        if (recvAckClientIdx < 0)
                        {
                            printf("Not Find Clinet.\n");
                        }

                        int chatRoomIdx = clientList[recvAckClientIdx].chatRoom;
                        if (chatRoomAck[chatRoomIdx] < chatRoomList[chatRoomIdx][0])
                        {
                            chatRoomAck[chatRoomIdx]++;
                        }

                        if (chatRoomAck[chatRoomIdx] >= chatRoomList[chatRoomIdx][0])
                        {
                            Server_Chat_Ack(clientList[recvAckClientIdx].fd, head.srcName, head.dstName);
                        }
                    }
                    else if (msgType == PACKET_TYPE_EXIT_REQ)
                    {
                        int flag = Server_Exit_Ack(clientList[i].fd, client);
                        if (flag == SERVER_FAIL)
                        {
                            printf("ACK FAIL.\n");
                        }
                        flag = Server_Del_Client_List(clientList[i].fd, client);
                        if (flag == SERVER_FAIL)
                        {
                            printf("DEL FAIL.\n");
                        }
                        curClientCnt -= 1;
                    }
                    else if (msgType == PACKET_TYPE_ALRAM_ACK)
                    {
                        
                        if(MallocQ->head != NULL && MallocQ->head->type == msgType)
                        {
                            Queue_Pop_Front();
                        }
                    }
                    else if (msgType == PACKET_TYPE_EXIT_ACK)
                    {
                        Server_Del_Client_List(clientList[i].fd, clientList[i].name);
                        curClientCnt -= 1;
                    }
                    else if (msgType == PACKET_TYPE_PING_ACK)
                    {
                        if (clientList[i].isPing == 1 && MallocQ->head != NULL && Queue_front()->type == PACKET_TYPE_PING_ACK)
                        {
                            clientList[i].isPing = 0;
                            Queue_Pop_Front();
                            printf("[%s] is Still Alive.\n", clientList[i].name);
                        }
                    }
                    else
                    {
                        printf("Packet Type is Unknown.\n");
                    }
                    fdNum--;
                }
            }
        }
    }
    int i = 0;
    for (i = 0; i < clientListSize; i++)
    {
        close(clientList[i].fd);
    }

    close(listenFD);
    return 0;
}