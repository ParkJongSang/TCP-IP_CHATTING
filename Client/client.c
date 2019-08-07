#include "./../Includes.h"
#include "clientFunction.h"

int main(void)
{
    char myName[20];
    char tryId[20];
    long timeWait = 0;
    int isConnected = 0;
    int isChat = 0;
    int isCommand = 1;
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
    signal(SIGINT, Client_Sigint_Handler);


    connectSocket.sin_family = AF_INET;
    inet_aton("127.0.0.1", (struct in_addr *)&connectSocket.sin_addr.s_addr);
    connectSocket.sin_port = htons(10000);
    connectFD = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(connectFD, F_SETFL, O_NONBLOCK);
    while (connect(connectFD, (struct sockaddr *)&connectSocket, sizeof(connectSocket)) == -1 && connectRetryCnt < 3)
    {
        if (errno == EISCONN)
            break;
        perror("[CONNECT]");
        connectRetryCnt += 1;
        sleep(1);
    }

    if(connectRetryCnt >= 3)
    {
        printf("You Cannot Connect to Server Because Some Error is Occured.\n");
        return -1;
    }

    ClientInit(10);

    while (1)
    {
        if(IS_SIG == 1)
        {
            printf("send Exit.\n");
            Client_Exit_Request(connectFD, myName);
            break;
        }

        int fdNum = 0;
        int i = 0;
        int fdMax = connectFD;
        int timeOverPacektType = -1;
        timeout.tv_sec = 0;
        timeout.tv_usec = 5000;
        fflush(stdin);
    
        if(isConnected == 0 && retryFlag == 1 && retryCnt < 3){
            printf("[%s]Retry.\n", tryId);
            Client_Connect_To_Server(connectFD, tryId);
        }
        retryFlag = 0;
        FD_ZERO(&reads);
        FD_SET(0, &reads);
        FD_SET(connectFD, &reads);
        if ((fdNum = select(fdMax + 1, &reads, 0, 0, &timeout)) == -1)
        {
            if(IS_SIG == 1)
            {
                printf("send Exit.\n");
                Client_Exit_Request(connectFD, myName);
                break;
            }
            perror("[SELECT]");
            break;
        }

        if (fdNum == 0)
        {
            if((timeOverPacektType = Client_Check_Packet_Time_Over(connectFD, myName, time(NULL))) == PACKET_TYPE_CONN_ACK){
                retryFlag = 1;
                retryCnt = 0;
            }
            continue;
        }
        if (FD_ISSET(0, &reads))
        {
            while(time(NULL) - timeWait < 1){
                continue;
            }
            char tmpStr[1024];
            fgets(tmpStr, 1024, stdin);
            if (isConnected == 1 && isChat == 0 && isCommand == 1)
            {
                printf("[%s]Command :: ", myName);
            }
            if (isConnected == 1 && isChat == 1 && isCommand == 1)
            {
                printf("[%s]Message :: ", myName);
            }
            if (strcmp(tmpStr, "SignUp\n") == 0)
            {
                char name[20];
                char lettering[20];
                char rejectCall[20];
                char callForwarding[20];

                printf("name :: ");
                scanf("%s", name);
                Client_Sign_Up(connectFD, name);
                printf("lettering :: ");
                scanf("%s", lettering);
                Client_Add_Lettering(connectFD, name, lettering);
                printf("rejectCall :: ");
                scanf("%s", rejectCall);
                Client_Add_Reject_Call(connectFD, name, rejectCall);
                printf("callForwarding :: ");
                scanf("%s", callForwarding);
                Client_Add_Call_Forwarding(connectFD, name, callForwarding);
            }
            else if (strcmp(tmpStr, "Connect\n") == 0 && isConnected == 0)
            {
                char tmpId[20];
                printf("ID :: ");
                scanf("%s", tmpId);
                strcpy(tryId, tmpId);
                Client_Connect_To_Server(connectFD, tryId);
            }
            else if (strcmp(tmpStr, "Exit\n") == 0)
            {
                Client_Exit_Request(connectFD, myName);
            }
            else if (strcmp(tmpStr, "AddLettering\n") == 0 && isConnected == 1)
            {
                char LetteringName[20];
                printf("LetteringName :: ");
                scanf("%s", LetteringName);
                Client_Add_Lettering(connectFD, myName, LetteringName);
            }
            else if (strcmp(tmpStr, "DelLettering\n") == 0 && isConnected == 1)
            {
                printf("Delete Your Lettering Name.\n");
                Client_Del_Lettering(connectFD, myName);
            }
            else if (strcmp(tmpStr, "AddCallForwarding\n") == 0 && isConnected == 1)
            {
                char CallForwardingName[20];
                printf("CallForwardingName :: ");
                scanf("%s", CallForwardingName);
                Client_Add_Call_Forwarding(connectFD, myName, CallForwardingName);
            }
            else if (strcmp(tmpStr, "DelCallForwarding\n") == 0 && isConnected == 1)
            {
                printf("Delete Your Call Forwarding Service.\n");
                Client_Del_Call_Forwarding(connectFD, myName);
            }
            else if (strcmp(tmpStr, "AddRejectCall\n") == 0 && isConnected == 1)
            {
                char RejectName[20];
                printf("RejectName :: ");
                scanf("%s", RejectName);
                Client_Add_Reject_Call(connectFD, myName, RejectName);
            }
            else if (strcmp(tmpStr, "DelRejectCall\n") == 0 && isConnected == 1)
            {
                char RejectName[20];
                printf("RejectName :: ");
                scanf("%s", RejectName);
                Client_Del_Reject_Call(connectFD, myName, RejectName);
            }
            else if (isConnected == 1 && isChat == 0 && strcmp(tmpStr, "Create\n") == 0)
            {
                char client[20];
                strcpy(client, myName);
                isCommand = 0;
                Client_Create_ChatRoom(connectFD, client);
            }
            else if (isConnected == 1 && isChat == 1)
            {
                char client[20];
                char tmpMsg[1024];
                strcpy(client, myName);
                if (strcmp(tmpStr, "!Invite\n") == 0)
                {
                    printf("Invite Name :: ");
                    scanf("%s", tmpMsg);
                    Client_Invite_Client_Request(connectFD, client, tmpMsg);
                }
                else if (strcmp(tmpStr, "!Exit\n") == 0)
                {
                    printf("Exit this chatRoom.\n");
                    Client_Exit_Chat_Room_Request(connectFD, client);
                }
                else if (strcmp(tmpStr, "\n") == 0)
                {
                    continue;
                }
                else
                {
                    Client_Chat_Request(connectFD, client, tmpStr);
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
                continue;
            }
            if (readBytes == 0)
            {
                printf("[FATAL_ERROR] : SERVER.\n");
                continue;
            }
            if ((readBytes = readPacket(connectFD, &body, ntohl(head.bodyLength))) != ntohl(head.bodyLength))
            {
                continue;
            }

            int msgType = ntohl(head.type);

            /* WAIT ACK */
            if (msgType == PACKET_TYPE_SIGN_UP_ACK)
            {
                printf("recv SingUp Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_STR_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_EXIT_ACK)
            {
                printf("recv Exit Ack.\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
                printf("ByeBye.\n");
                isConnected = 0;
                break;
            }
            else if (msgType == PACKET_TYPE_LETTERING_ADD_ACK)
            {
                printf("recv LETTERING Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_LETTERING_DEL_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_CALL_FORWARDING_ADD_ACK)
            {
                printf("recv Forwarding Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_CALL_FORWARDING_DEL_ACK)
            {
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (msgType == PACKET_TYPE_REJECT_CALL_ADD_ACK)
            {
                printf("recv Reject Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (MallocQ->head != NULL && msgType == PACKET_TYPE_REJECT_CALL_DEL_ACK)
            {
                if (MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
            }
            else if (MallocQ->head != NULL && msgType == PACKET_TYPE_CONN_ACK)
            {
                printf("recv Connect Ack\n");
                if (MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
                if (strcmp(body.str, "YES") == 0)
                {
                    isConnected = 1;
                    retryCnt = 0;
                    printf("Welcome %s\n", head.dstName);
                    strcpy(myName, head.dstName);
                }
                else
                {
                    printf("Not Matched Id\n");
                }
            }
            else if (msgType == PAKCET_TYPE_CREATE_CHATROOM_ACK)
            {
                printf("recv Create ChatRoom Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
                isCommand = 1;
                isChat = 1;
            }
            else if (msgType == PACKET_TYPE_EXIT_CHATROOM_ACK)
            {
                printf("recv Exit Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
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
            else if (msgType == PACKET_TYPE_INVITE_ACK)
            {
                printf("recv Invite Ack\n");
                if (MallocQ->head != NULL && MallocQ->head->type == msgType)
                {
                    Queue_Pop_Front();
                }
                if (strcmp(body.str, "yes") == 0)
                {
                    printf("[%s] Invite Success.\n", head.dstName);
                }
                else
                {
                    printf("[%s] Invite Fail.\n", head.dstName);
                }
            }

            /* Receive Request */
            if (msgType == PACKET_TYPE_PING_REQ)
            {
                Client_Ping_Ack(connectFD, myName);
            }
            else if (msgType == PACKET_TYPE_EXIT_REQ)
            {
                isChat = 0;
                isConnected = 0;
                Client_Exit_Ack(connectFD, myName);
                memset(myName, 0x00, sizeof(myName));
                break;
            }
            else if (msgType == PACKET_TYPE_STR_REQ)
            {
                /* PRINT MESSAGE */
                printf("<%s> %s", head.srcName, body.str);
                /* SEND ACK MESSAGE */
                Client_Chat_Ack(connectFD, head.srcName, myName);
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
                Client_Invite_Client_Ack(connectFD, head.srcName, head.dstName, msg);
            }
            else if (msgType == PACKET_TYPE_ALRAM_REQ)
            {
                /* PRINT ALRAM */
                printf("%s\n", body.str);
                /* SEND ACK MESSAGE */
                Client_Alram_Ack(connectFD, myName);
            }
            continue;
        }
    }
    printf("Close This Client.\n");
    close(connectFD);
    return 0;
}

