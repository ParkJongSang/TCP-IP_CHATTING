#include "User.h"

int User_Init_List()
{
    /* DATABASE LOADING */
    int i = 0;
    char tmpStr[64];
    FILE *fp;
    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/SubscriberTable.txt", "r")) == NULL)
    {
        perror("[FOPEN]:");
        return USER_FAIL;
    }
    memset(tmpStr, 0x00, sizeof(tmpStr));

    /* Input Array */
    while (fgets(tmpStr, sizeof(tmpStr), fp) != NULL)
    {
        int len = strlen(tmpStr);
        for (i = 0; i < len; i++)
        {
            if (tmpStr[i] == '\n')
            {
                break;
            }else{
                user[userSize][i] = tmpStr[i];
            }

        }
        size_t lastIdx = strlen(user[userSize]);
        if(user[userSize][lastIdx-1] == '\n'){
            user[userSize][lastIdx-1] = '\0';
        }
        userSize++;
    }
    /* Input Hash */
    for (i = 0; i < userSize; i++)
    {
        unsigned int key = User_Get_Key(user[i]);
        int size = UserHashTable[key][0];
        UserHashTable[key][size + 1] = i;
        UserHashTable[key][0]++;
    }
    /* CLOSE FILE */
    fclose(fp);
    return USER_SUCCESS;
}

int User_Del_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return USER_FAIL;
    }

    int idx = User_Search_List(name);
    int search = 0;
    int i = 0;
    unsigned int key = User_Get_Key(name);
    int size = UserHashTable[key][0];

    if (idx < 0)
    {
        return USER_FAIL;
    }

    for (i = 1; i <= size; i++)
    {
        int hashToIdx = UserHashTable[key][i];
        if (search == 0 && strcmp(name, user[hashToIdx]) == 0)
        {
            search = 1;
        }
        if (search == 1 && i < size)
        {
            UserHashTable[key][i] = UserHashTable[key][i + 1];
        }
    }
    if (search == 0)
    {
        return USER_FAIL;
    }
    UserHashTable[key][0] -= 1;

    for (i = idx; i < userSize; i++)
    {
        memset(user[i], 0x00, sizeof(user[i]));
        strcpy(user[i], user[i + 1]);
    }
    memset(user[userSize - 1], 0x00, sizeof(user[userSize - 1]));
    userSize -= 1;

    return USER_SUCCESS;
}

int User_Add_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return USER_FAIL;
    }

    if (User_Search_List(name) >= 0)
    {
        return USER_FAIL;
    }

    int toAddIdx = userSize;
    memset(user[toAddIdx], 0x00, sizeof(user[toAddIdx]));
    strcpy(user[toAddIdx], name);

    unsigned int key = User_Get_Key(name);
    int size = UserHashTable[key][0];
    UserHashTable[key][size + 1] = toAddIdx;
    UserHashTable[key][0] += 1;

    userSize++;
    return USER_SUCCESS;
}

int User_Search_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return USER_FAIL;
    }

    unsigned int key = User_Get_Key(name);
    int size = UserHashTable[key][0];
    int i = 0;

    for (i = 1; i <= size; i++)
    {
        int hashToIdx = UserHashTable[key][i];
        if (strcmp(name, user[hashToIdx]) == 0)
        {
            return hashToIdx;
        }
    }
    printf("NO SEARCH\n");
    return USER_FAIL;
}

int User_Save_File()
{
    FILE *fp;
    int i = 0;

    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/SubscriberTable.txt", "w")) == NULL)
    {
        perror("[FOPEN]:");
        return USER_FAIL;
    }

    for (i = 0; i < userSize; i++)
    {
        fprintf(fp, "%s\n", user[i]);
    }

    fclose(fp);

    return USER_SUCCESS;
}

unsigned int User_Get_Key(char *name)
{
    unsigned int ret = 0;
    int toMul = 1;
    int i = 0;
    for (i = 0; name[i] != '\0'; i++)
    {
        ret += ((int)name[i] * toMul) % 100;
        toMul *= USER_PN;
    }
    return ret % 100;
}