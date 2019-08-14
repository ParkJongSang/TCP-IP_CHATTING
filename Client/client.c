#include "./../Includes.h"
#include "clientFunction.h"

int main(void)
{
    char myName[20];
    char tryId[20];
    long timeWait = 0;
    int isConnected = 0;
    int isChat = 0;
    int retryCnt = 0;
    int retryFlag = 0;
    int connectRetryCnt = 0;
    int connectFD;
    struct timeval timeout;
    fd_set reads;

    packetBody_t body;
    packetHead_t head;

    struct sockaddr_in connectSocket;

    memset(&connectSocket, 0x00, sizeof(connectSocket));
    memset(&body, 0x00, sizeof(body));
    memset(&head, 0x00, sizeof(head));

    IS_SIG = 0;
    
    if (signal(SIGINT, Client_Sigint_Handler) < 0)
    {
        printf("Set Signal Handler(SIGING) Error.\n");
    }
    if (signal(SIGPIPE, SIG_IGN) < 0)
    {
        printf("Set Signal Hnadler(SIGPIPE) Error.\n");
    }

    connectSocket.sin_family = AF_INET;
    inet_aton("127.0.0.1", (struct in_addr *)&connectSocket.sin_addr.s_addr);
    connectSocket.sin_port = htons(10000);
    connectFD = socket(AF_INET, SOCK_STREAM, 0);
    if (fcntl(connectFD, F_SETFL, O_NONBLOCK) < 0)
    {
        printf("Set Socket Function(NON_BLOCK) Error.\n");
    }
    while (connect(connectFD, (struct sockaddr *)&connectSocket, sizeof(connectSocket)) == -1 && connectRetryCnt < 3)
    {
        if (errno == EISCONN)
            break;
        perror("[CONNECT]");
        connectRetryCnt += 1;
        sleep(1);
    }

    if (connectRetryCnt >= 3)
    {
        printf("You Cannot Connect to Server Because Some Error is Occured.\n");
        return -1;
    }

    ClientInit(10);

    while (1)
    {
        if (IS_SIG == 1)
        {
            printf("send Exit.\n");
            if (Client_Exit_Request(connectFD, myName) == CLIENT_FAIL)
            {
                printf("Exit Request Fail.\n");
            }
            break;
        }

        int fdNum = 0;
        int i = 0;
        int fdMax = connectFD;
        int timeOverPacektType = -1;
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;

        if (isConnected == 0 && retryFlag == 1 && retryCnt < 3)
        {
            printf("[%s]Retry.\n", tryId);
            if (Client_Connect_To_Server(connectFD, tryId) == CLIENT_FAIL)
            {
                printf("Connect To Server Error.\n");
            }
        }

        retryFlag = 0;
        FD_ZERO(&reads);
        FD_SET(0, &reads);
        FD_SET(connectFD, &reads);

        if ((fdNum = select(fdMax + 1, &reads, 0, 0, &timeout)) == -1)
        {
            if (IS_SIG == 1)
            {
                printf("send Exit.\n");
                if (Client_Exit_Request(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Exit Request Fail.\n");
                }
                break;
            }
            perror("[SELECT]");
            break;
        }

        if (fdNum == 0)
        {
            if ((timeOverPacektType = Client_Check_Packet_Time_Over(connectFD, myName, time(NULL))) == PACKET_TYPE_CONN_ACK)
            {
                retryFlag = 1;
                retryCnt = 0;
            }
            continue;
        }

        if (FD_ISSET(0, &reads))
        {
            while (time(NULL) - timeWait < 1)
            {
                continue;
            }
            char tmpStr[1024];
            fgets(tmpStr, 1024, stdin);
            if (strcmp(tmpStr, "Help\n") == 0 || strcmp(tmpStr, "help\n") == 0)
            {
                printf("=====Introduce Command=====\n");
                printf("1. SignUp - Sign Up this Service\n");
                printf("2. Connect - Connect with your id\n");
                printf("3. Exit - Exit this program\n");

                printf("=====After Connect=====\n");
                printf("1. Create - Create Chatroom\n");
                printf("2. AddLettering - Add Lettering Service\n");
                printf("3. AddRejectCall - Add Reject Call Service\n");
                printf("4. AddCallForwarding - Add Call Forwarding\n");
                printf("5. DelLettering - Delete Lettering Service\n");
                printf("6. DelRejectCall - Delete Reject Call Service\n");
                printf("7. DelCallForwarding - Delete Call Forwarding Service\n");

                printf("=====In Chatroom=====\n");
                printf("1. !Invite - Invite other user\n");
                printf("2. !Exit - Exit this Chatroom\n");
            }
            else if (strcmp(tmpStr, "SignUp\n") == 0)
            {
                char name[20];
                char lettering[20];
                char rejectCall[20];
                char callForwarding[20];

                printf("name :: ");
                scanf("%s", name);
                if (Client_Sign_Up(connectFD, name) == CLIENT_FAIL)
                {
                    printf("Sign Up Req Fail.\n");
                }
                printf("lettering :: ");
                scanf("%s", lettering);
                if (Client_Add_Lettering(connectFD, name, lettering) == CLIENT_FAIL)
                {
                    printf("Lettering Add Req Fail.\n");
                }
                printf("rejectCall :: ");
                scanf("%s", rejectCall);
                if (Client_Add_Reject_Call(connectFD, name, rejectCall) == CLIENT_FAIL)
                {
                    printf("RejectCall Add Req Fail.\n");
                }
                printf("callForwarding :: ");
                scanf("%s", callForwarding);
                if (Client_Add_Call_Forwarding(connectFD, name, callForwarding) == CLIENT_FAIL)
                {
                    printf("Call Forwarding Add Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "Connect\n") == 0 && isConnected == 0 && isChat == 0)
            {
                char tmpId[20];
                printf("ID :: ");
                scanf("%s", tmpId);
                if (strcpy(tryId, tmpId) == NULL)
                {
                    printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                }
                if (Client_Connect_To_Server(connectFD, tryId) == CLIENT_FAIL)
                {
                    printf("Connect Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "Exit\n") == 0)
            {
                if (Client_Exit_Request(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Exit Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "AddLettering\n") == 0 && isConnected == 1 && isChat == 0)
            {
                char LetteringName[20];
                printf("LetteringName :: ");
                scanf("%s", LetteringName);
                if (Client_Add_Lettering(connectFD, myName, LetteringName) == CLIENT_FAIL)
                {
                    printf("Lettering Add Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "DelLettering\n") == 0 && isConnected == 1 && isChat == 0)
            {
                printf("Delete Your Lettering Name.\n");
                if (Client_Del_Lettering(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Lettering Del Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "AddCallForwarding\n") == 0 && isConnected == 1 && isChat == 0)
            {
                char CallForwardingName[20];
                printf("CallForwardingName :: ");
                scanf("%s", CallForwardingName);
                if (Client_Add_Call_Forwarding(connectFD, myName, CallForwardingName) == CLIENT_FAIL)
                {
                    printf("Call Forwarding Add Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "DelCallForwarding\n") == 0 && isConnected == 1 && isChat == 0)
            {
                printf("Delete Your Call Forwarding Service.\n");
                if (Client_Del_Call_Forwarding(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Call Forwarding Del Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "AddRejectCall\n") == 0 && isConnected == 1 && isChat == 0)
            {
                char RejectName[20];
                printf("RejectName :: ");
                scanf("%s", RejectName);
                if (Client_Add_Reject_Call(connectFD, myName, RejectName) == CLIENT_FAIL)
                {
                    printf("Reject Call Add Req Fail.\n");
                }
            }
            else if (strcmp(tmpStr, "DelRejectCall\n") == 0 && isConnected == 1 && isChat == 0)
            {
                char RejectName[20];
                printf("RejectName :: ");
                scanf("%s", RejectName);
                if (Client_Del_Reject_Call(connectFD, myName, RejectName) == CLIENT_FAIL)
                {
                    printf("Reject Call Del Req Fail.\n");
                }
            }
            else if (isConnected == 1 && isChat == 0 && strcmp(tmpStr, "Create\n") == 0)
            {
                char client[20];
                if (strcpy(client, myName) == NULL)
                {
                    printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                }
                if (Client_Create_ChatRoom(connectFD, client) == CLIENT_FAIL)
                {
                    printf("Create Chatromm Req Fail.\n");
                }
            }
            else if (isConnected == 1 && isChat == 1)
            {
                char client[20];
                char tmpMsg[1024];
                if (strcpy(client, myName) == NULL)
                {
                    printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                }
                if (strcmp(tmpStr, "!Invite\n") == 0)
                {
                    printf("Invite Name :: ");
                    scanf("%s", tmpMsg);
                    if (Client_Invite_Client_Request(connectFD, client, tmpMsg) == CLIENT_FAIL)
                    {
                        printf("Invite Req Fail.\n");
                    }
                }
                else if (strcmp(tmpStr, "!Exit\n") == 0)
                {
                    printf("Exit this chatRoom.\n");
                    if (Client_Exit_Chat_Room_Request(connectFD, client) == CLIENT_FAIL)
                    {
                        printf("Exit Chatroom Req Fail.\n");
                    }
                }
                else if (strcmp(tmpStr, "!User\n") == 0)
                {
                    if (Client_All_Ueser_Request(connectFD, client) == CLIENT_FAIL)
                    {
                        printf("All User Request Fail.\n");
                    }
                }
                else if (strcmp(tmpStr, "\n") == 0)
                {
                    continue;
                }
                else
                {
                    if (Client_Chat_Request(connectFD, client, tmpStr) == CLIENT_FAIL)
                    {
                        printf("Chat Req Fail.\n");
                    }
                }
            }
            else
            {
                printf("RE\n");
            }

            timeWait = time(NULL);
            continue;
        }

        if (FD_ISSET(connectFD, &reads))
        {
            size_t readBytes;
            memset(&head, 0x00, sizeof(head));
            memset(&body, 0x00, sizeof(body));
            if ((readBytes = readPacket(connectFD, &head, sizeof(head))) != sizeof(head))
            {
                printf("Read Header Error.\n");
                continue;
            }
            if (readBytes == 0)
            {
                printf("[FATAL_ERROR] : SERVER.\n");
                continue;
            }
            if ((readBytes = readPacket(connectFD, &body, ntohl(head.bodyLength))) != ntohl(head.bodyLength))
            {
                printf("Read Body Error.\n");
                continue;
            }

            int msgType = ntohl(head.type);

            /* WAIT ACK */
            if (msgType == PACKET_TYPE_SIGN_UP_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_STR_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_EXIT_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                printf("ByeBye.\n");
                isConnected = 0;
                break;
            }
            else if (msgType == PACKET_TYPE_LETTERING_ADD_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_LETTERING_DEL_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_CALL_FORWARDING_ADD_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_CALL_FORWARDING_DEL_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_REJECT_CALL_ADD_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_REJECT_CALL_DEL_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
            }
            else if (msgType == PACKET_TYPE_CONN_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                if (strcmp(body.str, "YES") == 0)
                {
                    isConnected = 1;
                    retryCnt = 0;
                    printf("Welcome %s\n", head.dstName);
                    if (strcpy(myName, head.dstName) == NULL)
                    {
                        printf("(%d)Dest String Pointer Is NULL.\n", __LINE__);
                    }
                }
                else
                {
                    printf("Not Matched Id\n");
                }
            }
            else if (MallocQ->head != NULL && msgType == PAKCET_TYPE_CREATE_CHATROOM_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                isChat = 1;
            }
            else if (MallocQ->head != NULL && msgType == PACKET_TYPE_EXIT_CHATROOM_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                if (strcmp(body.str, "yes") == 0)
                {
                    printf("[%s] Invite Success.\n", head.dstName);
                }
                else
                {
                    printf("[%s] Invite Fail.\n", head.dstName);
                }
                isChat = 0;
            }
            else if (MallocQ->head != NULL && msgType == PACKET_TYPE_INVITE_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                if (strcmp(body.str, "yes") == 0)
                {
                    printf("[%s] Invite Success.\n", head.dstName);
                }
                else
                {
                    printf("[%s] %s\n", head.dstName, body.str);
                    printf("[%s] Invite Fail.\n", head.dstName);
                }
            }
            else if (MallocQ->head != NULL && msgType == PACKET_TYPE_ALL_USER_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    if (Queue_Pop_Front() == QUEUE_FAIL)
                    {
                        printf("Queue Pop Fail.\n");
                    }
                }
                printf("%s", body.str);
            }

            /* Receive Request */
            if (msgType == PACKET_TYPE_PING_REQ)
            {
                printf("RECV PING.\n");
                if (Client_Ping_Ack(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Send Ping Ack Fail.\n");
                }
            }
            else if (msgType == PACKET_TYPE_EXIT_REQ)
            {
                isChat = 0;
                isConnected = 0;

                if (Client_Exit_Ack(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Send Exit Ack Fail.\n");
                }

                memset(myName, 0x00, sizeof(myName));
                break;
            }
            else if (msgType == PACKET_TYPE_STR_REQ)
            {
                /* PRINT MESSAGE */
                printf("<%s> %s", head.srcName, body.str);
                /* SEND ACK MESSAGE */
                if (Client_Chat_Ack(connectFD, head.srcName, myName) == CLIENT_FAIL)
                {
                    printf("Send Chat Ack Fail.\n");
                }
            }
            else if (msgType == PACKET_TYPE_INVITE_REQ)
            {
                /* REQUEST ACCEPT */
                char cmd[3];
                char msg[10];
                printf("[%s] Invite you. Accept? (y/n) : ", head.srcName);
                scanf("%s", cmd);
                if (strcmp(cmd, "y") == 0 || strcmp(cmd, "Y") == 0)
                {
                    strcpy(msg, "yes");
                }
                else
                {
                    strcpy(msg, "no");
                }
                isChat = 1;
                if (Client_Invite_Client_Ack(connectFD, head.srcName, head.dstName, msg) == CLIENT_FAIL)
                {
                    printf("Send Invite Ack Fail.\n");
                }
            }
            else if (msgType == PACKET_TYPE_ALRAM_REQ)
            {
                /* PRINT ALRAM */
                printf("%s\n", body.str);
                /* SEND ACK MESSAGE */
                if (Client_Alram_Ack(connectFD, myName) == CLIENT_FAIL)
                {
                    printf("Send Alram Ack Message Fail.\n");
                }
            }
            continue;
        }
    }
    printf("Close This Client.\n");
    Queue_Free();
    close(connectFD);
    return 0;
}
