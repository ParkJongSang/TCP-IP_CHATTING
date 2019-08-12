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
    if (signal(SIGINT, Server_Siging_Handler) < 0)
    {
        printf("Set Signal Handler(SIGINT) Error.\n");
    }
    if (signal(SIGPIPE, SIG_IGN) < 0)
    {
        printf("Set Signal Handler(SIGPIPE) Error.\n");
    }
    memset(&listenSocket, 0x00, sizeof(listenSocket));
    memset(&body, 0x00, sizeof(body));
    memset(&head, 0x00, sizeof(head));

    listenSocket.sin_family = AF_INET;
    listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
    listenSocket.sin_port = htons(10000);

    ServerInit(100);
    if (setsockopt(listenFD, SOL_SOCKET, SO_REUSEPORT, (char *)&reuseFlag, sizeof(reuseFlag)) < 0)
    {
        printf("Set Socket Option(Reusable) Error.\n");
    }
    if (fcntl(listenFD, F_SETFL, O_NONBLOCK) < 0)
    {
        printF("Set Socket Function(NON_BLOCK) Error.\n");
    }

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
                if (Server_Exit_Request(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                {
                    printf("Send Exit Request Fail.\n");
                }
            }
            break;
        }

        FD_ZERO(&reads);
        FD_SET(0, &reads);
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
                    if (Server_Exit_Request(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                    {
                        printf("Send Exit Request Fail.\n");
                    }
                }
                break;
            }
            break;
        }
        if (fdNum == 0)
        {
            /* Add time on Every Wait Queue Value */
            if (Server_Check_Client_Time_Over(time(NULL)) == SERVER_FAIL)
            {
                printf("Client Time Check Fail.\n");
            }
            Server_Check_Packet_Time_Over(time(NULL));
            if (IS_SIG == 1)
            {
                for (i = 0; i < clientListSize; i++)
                {
                    if (Server_Exit_Request(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                    {
                        printf("Send Exit Request Fail.\n");
                    }
                }
                break;
            }
            continue;
        }
        if (FD_ISSET(0, &reads) cd)
        {
            char tmpStr[1024];
            fgets(tmpStr, 1024, stdin);
            if (strcmp(tmpStr, "AddUser\n") == 0)
            {
                char name[20];
                printf("name :: ");
                scanf("%s", name);

                int clientIdx = Server_Search_Client(name);

                if (User_Add_List(name) == USER_FAIL || clientIdx >= 0)
                {
                    printf("User Add Fail.\n");
                }
                if (User_Save_File() == USER_FAIL)
                {
                    printf("User Save File Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "DelUser\n") == 0)
            {
                char name[20];
                printf("name :: ");
                scanf("%s", name);

                int clientIdx = Server_Search_Client(name);

                if (User_Del_List(name) == USER_FAIL || clientIdx >= 0)
                {
                    printf("User Del Fail.\n");
                }
                if (User_Save_File() == USER_FAIL)
                {
                    printf("User Save File Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "ModifyUser\n") == 0)
            {
                char originName[20], changeName[20];
                int flag = 1;

                printf("Origin Name :: ");
                scanf("%s", originName);
                printf("Change Name :: ");
                scanf("%s", changeName);

                int originIdx = Server_Search_Client(originName);
                int changeIndx = User_Search_List(changeName);

                if (originIdx < 0)
                {
                    if (User_Del_List(originName) == USER_FAIL)
                    {
                        flag = 0;
                        printf("User Modify Fail.(Del)\n");
                    }
                }
                else
                {
                    printf("Client is conneting now.\n");
                }

                if (flag == 1 && changeIndx < 0)
                {
                    if (User_Add_List(changeName) == USER_FAIL)
                    {
                        printf("User Modify Fail.(Add)\n");
                    }
                }
                else
                {
                    printf("Change Name is using.\n");
                }
                if (User_Save_File() == USER_FAIL)
                {
                    printf("User Save File Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "PrintUser\n") == 0)
            {
                int length = userSize;
                int i = 0;
                printf("======User List======\n");
                for (i = 0; i < length; i++)
                {
                    printf("%d.%s\n", i + 1, user[i]);
                }
            }
            else if (strcmp(tmpStr, "Help\n") == 0 || strcmp(tmpStr, "help\n") == 0)
            {
                printf("======command=======\n");
                printf("1. AddUser - Add User Name.\n");
                printf("2. DelUser - Delete User Name.\n");
                printf("3. ModifyUser - Modify User Name.\n");
                printf("4. PrintUser - Print All Users.\n");
            }
            fdNum -= 1;
        }

        if (FD_ISSET(listenFD, &reads))
        {
            char tmpId[20];
            socklen_t addrSize = sizeof(connectSocket);
            while ((connectFD = accept(listenFD, (struct sockaddr *)&connectSocket, &addrSize)) < 0)
            {
                sleep(0.5);
            }
            sprintf(tmpId, "unknown%d", clientListSize);
            if (Server_Add_Client_List(connectFD, tmpId, time(NULL)) == SERVER_FAIL)

                printf("add Fail.\n");
        }
        if (fcntl(connectFD, F_SETFL, O_NONBLOCK) == -1)
        {
            printf("FCNTL ERROR.\n");
        }
        printf("[CONNECTED] : %s CLIENT\n", tmpId);
        fdNum -= 1;

        if (clientListSize > 10)
        {
            if (Server_Exit_Request(connectFD, tmpId) == USER_FAIL)
            {
                printf("Send Exit Req Fail.\n");
            }
            continue;
        }
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
                    printf("Read Header Error.\n");
                    fdNum--;
                    continue;
                }
                if ((readBytes = readPacket(clientList[i].fd, &body, ntohl(head.bodyLength))) != ntohl(head.bodyLength))
                {
                    printf("Read Header Error.\n");
                    fdNum--;
                    continue;
                }

                int msgType = ntohl(head.type);

                if (msgType == PACKET_TYPE_SIGN_UP_REQ)
                {
                    if (strcpy(client, body.str) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    if (User_Add_List(body.str) == USER_FAIL)
                    {
                        printf("USER ADD FAIL\n");
                    }
                    if (User_Save_File() == USER_FAIL)
                    {
                        printf("USER SAVE FAIL\n");
                    }
                    if (Server_Sign_Up_Ack(clientList[i].fd, client) == SERVER_FAIL)
                    {
                        printf("Send Sign Up Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_LETTERING_ADD_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    if (Lettering_Add_List(client, body.str) == LETTERING_FAIL)
                    {
                        printf("Lettering Add Fail.\n");
                    }
                    if (Lettering_Save_File() == LETTERING_FAIL)
                    {
                        printf("Lettering Save File Fai.\n");
                    }
                    if (Server_Add_Lettering_Ack(clientList[i].fd, client) == SERVER_FAIL)
                    {
                        printf("Send Add Lettering Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_REJECT_CALL_ADD_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    if (RejectCall_Add_List(client, body.str) == REJECT_CALL_FAIL)
                    {
                        printf("Reject Call Add Fail.\n");
                    }
                    if (RejectCall_Save_File() == REJECT_CALL_FAIL)
                    {
                        printf("Reject Call Save File Fail.\n");
                    }
                    if (Server_Add_Reject_Call_Ack(clientList[i].fd, client) == REJECT_CALL_FAIL)
                    {
                        printf("Send Reject Call Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_CALL_FORWARDING_ADD_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    if (CallForwarding_Add_List(client, body.str) == CALL_FORWARDING_FAIL)
                    {
                        printf("Call Forwarding Add Fail.\n");
                    }
                    if (CallForwarding_Save_File() == CALL_FORWARDING_FAIL)
                    {
                        printf("Call Forwardig Save File Fail.\n");
                    }
                    if (Server_Add_Call_Forwarding_Ack(clientList[i].fd, client) == CALL_FORWARDING_FAIL)
                    {
                        printf("Send Call Forwarding Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_CONN_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
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

                    if (Server_Ack_To_Client(clientList[i].fd, client, state) == SERVER_FAIL)
                    {
                        printf("Send Ack for Connect.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_EXIT_REQ)
                {
                    int curChatRoom = -1;
                    int curClientInChatRoom = -1;
                    int j = 0;
                    int searchFlag = 0;
                    char msg[1024];

                    if (clientList[i].chatRoom != 0)
                    {
                        curChatRoom = clientList[i].chatRoom;
                        curClientInChatRoom = chatRoomList[curChatRoom][0];
                    }

                    if (curChatRoom != -1 && curClientInChatRoom != -1)
                    {
                        sprintf(msg, "[%s] is Exit Chatroom.", clientList[i].name);
                        if (strcpy(client, head.srcName) == NULL)
                        {
                            printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                        }
                        for (j = 1; j <= curClientInChatRoom; j++)
                        {
                            if (clientList[i].fd != chatRoomList[curChatRoom][j])
                            {
                                if (Server_Alram_Request(chatRoomList[curChatRoom][j], msg) == SERVER_FAIL)
                                {
                                    printf("Send Alram Req Fail.\n");
                                }
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

                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    if (Server_Exit_Ack(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                    {
                        printf("Send Exit Ack Fail.\n");
                    }
                    if (Server_Del_Client_List(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                    {
                        printf("Delete Client in ClientList Fail.\n");
                    }
                    curClientCnt -= 1;
                }
                else if (msgType == PACKET_TYPE_EXIT_CHATROOM_REQ)
                {

                    int curChatRoom = clientList[i].chatRoom;
                    int curClientInChatRoom = chatRoomList[curChatRoom][0];
                    int j = 0;
                    int searchFlag = 0;
                    char msg[1024];

                    sprintf(msg, "[%s] is Exit Chatroom.", clientList[i].name);

                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    /* Send All Client in Same ChatRooms */
                    if (Server_Exit_Chat_Romm_Ack(clientList[i].fd, client) == SERVER_FAIL)
                    {
                        printf("Send Exit Chatroom Ack Fail.\n");
                    }

                    for (j = 1; j <= curClientInChatRoom; j++)
                    {
                        if (clientList[i].fd != chatRoomList[curChatRoom][j])
                        {
                            if (Server_Alram_Request(chatRoomList[curChatRoom][j], msg) == SERVER_FAIL)
                            {
                                printf("Send Alram Fail.\n");
                            }
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
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    /* Del */
                    if (Lettering_Del_List(client) == LETTERING_FAIL)
                    {
                        printf("Lettering Del Fail.\n");
                    }
                    if (Lettering_Save_File() == LETTERING_FAIL)
                    {
                        printf("Lettering Save File Fail.\n");
                    }
                    if (Server_Del_Lettering_Ack(clientList[i].fd, client) == SERVER_FAIL)
                    {
                        printf("Send Del Lettering Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_REJECT_CALL_DEL_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    /* Del */
                    if (RejectCall_Del_List(client, body.str) == REJECT_CALL_FAIL)
                    {
                        printf("RejectCall Del Fail.\n");
                    }
                    if (RejectCall_Save_File() == REJECT_CALL_FAIL)
                    {
                        printf("RejectCall Save File Fail.\n");
                    }
                    if (Server_Del_Reject_Call_Ack(clientList[i].fd, client) == REJECT_CALL_FAIL)
                    {
                        printf("Send Del RejectCall Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_CALL_FORWARDING_DEL_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    /* Del */
                    if (CallForwarding_Del_List(client) == CALL_FORWARDING_FAIL)
                    {
                        printf("Call Forwarding Del Fail.\n");
                    }
                    if (CallForwarding_Save_File() == CALL_FORWARDING_FAIL)
                    {
                        printf("Call Forwarding Save File Fail.\n");
                    }
                    if (Server_Del_Call_Forwarding_Ack(clientList[i].fd, client) == CALL_FORWARDING_FAIL)
                    {
                        printf("Send Del Call Forwarding Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_CREATE_CHATROOM_REQ)
                {
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
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

                    if (Server_Create_Chat_Room_Ack(clientList[i].fd, client) == SERVER_FAIL)
                    {
                        printf("Send Create Chatroom Ack Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_INVITE_REQ)
                {
                    /* INVITE LOGIC */
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
                        if (strcpy(head.dstName, callForwarding[isForward].dstName) == NULL)
                        {
                            printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                        }
                    }

                    if (clientList[searchClientIdx].chatRoom > 0)
                    {
                        memset(body.str, 0x00, sizeof(body.str));
                        if (strcpy(body.str, "Client already chat with other Client.") == NULL)
                        {
                            printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                        }
                        if (Server_Invite_Client_Ack(clientList[i].fd, head.srcName, head.dstName, body.str) == SERVER_FAIL)
                        {
                            printf("Send Invite Ack Fail.\n");
                        }
                    }
                    else
                    {
                        if (Server_Invite_Client_Request(clientList[searchClientIdx].fd, head.srcName, head.dstName) == SERVER_FAIL)
                        {
                            printf("Send Invite Fail.\n");
                        }
                    }
                }
                else if (msgType == PACKET_TYPE_INVITE_ACK)
                {
                    /* INVITE LOGIC */
                    if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                    {
                        if (Queue_Pop_Front() == QUEUE_FAIL)
                        {
                            printf("Pop Fail.\n");
                        }
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
                        int j = 0;
                        char msg[1024];

                        clientList[beInvitedClientIdx].chatRoom = curChatRoom;
                        chatRoomList[curChatRoom][size + 1] = clientList[beInvitedClientIdx].fd;
                        chatRoomList[curChatRoom][0] += 1;

                        sprintf(msg, "[%s] is Invited Chat Room.\n", head.dstName);

                        for (j = 1; j <= size; j++)
                        {
                            if (clientList[i].fd != chatRoomList[curChatRoom][j])
                            {
                                if (Server_Alram_Request(chatRoomList[curChatRoom][j], msg) == SERVER_FAIL)
                                {
                                    printf("Send Alram Fail.\n");
                                }
                            }
                        }
                    }

                    if (Server_Invite_Client_Ack(clientList[searchClientIdx].fd, head.srcName, head.dstName, body.str) == SERVER_FAIL)
                    {
                        printf("Send Invite Client Ack Fail.\n");
                    }
                    printf("send Invite Complete.\n");
                }
                else if (msgType == PACKET_TYPE_STR_REQ)
                {
                    int curChatRoom = clientList[i].chatRoom;
                    int curClientInChatRoom = chatRoomList[curChatRoom][0];
                    int j = 0;
                    char msg[1024];
                    int letteringIdx;
                    if (strcpy(client, head.srcName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                    memset(msg, 0x00, sizeof(msg));
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
                            if (Server_Chat_Request(chatRoomList[curChatRoom][j], head.srcName, msg) == SERVER_FAIL)
                            {
                                printf("Send Chat Req Fail.\n");
                            }
                        }
                    }
                }
                else if (msgType == PACKET_TYPE_STR_ACK)
                {
                    if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                    {
                        if (Queue_Pop_Front() == QUEUE_FAIL)
                        {
                            printf("Pop Fail.\n");
                        }
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
                        if (Server_Chat_Ack(clientList[recvAckClientIdx].fd, head.srcName, head.dstName) == SERVER_FAIL)
                        {
                            printf("Send Chat Ack Fail.\n");
                        }
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
                    if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                    {
                        if (Queue_Pop_Front() == QUEUE_FAIL)
                        {
                            printf("Pop Fail.\n");
                        }
                    }
                }
                else if (msgType == PACKET_TYPE_EXIT_ACK)
                {
                    if (Server_Del_Client_List(clientList[i].fd, clientList[i].name) == SERVER_FAIL)
                    {
                        printf("Del Client Fail.\n");
                    }
                }
                else if (msgType == PACKET_TYPE_PING_ACK)
                {
                    if (clientList[i].isPing == 1 && MallocQ->head != NULL && MallocQ->head->type == PACKET_TYPE_PING_ACK && strcmp(MallocQ->head->name, clientList[i].name) == 0)
                    {
                        clientList[i].isPing = 0;
                        if (Queue_Pop_Front() == QUEUE_FAIL)
                        {
                            printf("Pop Fail.\n");
                        }
                        printf("[%s] is Still Alive.\n", clientList[i].name);
                    }
                }
                else if (msgType == PACKET_TYPE_ALL_USER_REQ)
                {
                    int j = 0;
                    char *msg[1024];
                    memset(msg, 0x00, sizeof(msg));
                    for (j = 0; j < clientListSize; j++)
                    {
                        strcat(msg, "[");
                        strcat(msg, i + 1);
                        strcat(msg, "]");
                        strcat(msg, clientList[j].name);
                        strcat(msg, "\n");
                    }
                    if (Server_All_User_Ack(clientList[i].fd, clientList[i].name, msg) == SERVER_FAIL)
                    {
                        printf("Send All User Ack Fail.\n");
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
Queue_Free();
close(listenFD);
return 0;
}