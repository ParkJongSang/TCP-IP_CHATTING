#include "CallForwarding.h"

int CallForwarding_Init_List()
{
    /* DATABASE LOADING */
    char tmpStr[128];
    FILE *fp;
    callForwardingSize = 0;

    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/CallForwardingTable.txt", "r")) == NULL)
    {
        perror("[FOPEN]:");
        return CALL_FORWARDING_FAIL;
    }

    memset(tmpStr, 0x00, sizeof(tmpStr));
    /* Input Array */
    while (fgets(tmpStr, sizeof(CallForwarding_t), fp) != NULL)
    {
        int len = strlen(tmpStr);
        int i = 0;
        int key_value = 0;
        char key[20], keyIdx = 0;
        char value[20], valueIdx = 0;

        memset(key, 0x00, sizeof(key));
        memset(value, 0x00, sizeof(value));

        for (i = 0; i < len; i++)
        {
            if (tmpStr[i] == ' ')
            {
                continue;
            }
            if (tmpStr[i] == ':')
            {
                key_value = 1;
                continue;
            }
            if (tmpStr[i] == '\n')
            {
                break;
            }
            if (key_value == 0)
            {
                key[keyIdx++] = tmpStr[i];
            }
            else
            {
                value[valueIdx++] = tmpStr[i];
            }
        }
        if(strlen(key) > 0 && strlen(value) > 0)
        {
            strcpy(callForwarding[callForwardingSize].srcName, key);
            strcpy(callForwarding[callForwardingSize].dstName, value);
            callForwardingSize += 1;
            memset(tmpStr, 0x00, sizeof(tmpStr));
        }
    }

    /* Input HashTable */
    int i = 0;
    for (i = 0; i < callForwardingSize; i++)
    {
        unsigned int key = CallForwarding_Get_Key(callForwarding[i].srcName);
        int size = callForwardingHashTable[key][0];
        callForwardingHashTable[key][size + 1] = i;
        callForwardingHashTable[key][0] += 1;
    }

    /* CLOSE FILE */
    fclose(fp);
    return CALL_FORWARDING_SUCCESS;
}

int CallForwarding_Del_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return CALL_FORWARDING_FAIL;
    }

    /* del in hashTable*/
    int idx = CallForwarding_Search_List(name);
    int search = 0;
    int i = 0;
    unsigned int key = CallForwarding_Get_Key(name);
    int size = callForwardingHashTable[key][0];

    if (idx < 0)
    {
        return CALL_FORWARDING_FAIL;
    }

    for (i = 1; i <= size; i++)
    {
        int hashToIdx = callForwardingHashTable[key][i];
        if (search == 0 && strcmp(name, callForwarding[hashToIdx].srcName) == 0)
        {
            search = 1;
        }
        if (search == 1 && i < size)
        {
            callForwardingHashTable[key][i] = callForwardingHashTable[key][i + 1];
        }
    }
    if (search == 0)
    {
        return CALL_FORWARDING_FAIL;
    }
    callForwardingHashTable[key][0] -= 1;

    /* del in array */
    for (i = idx; i < callForwardingSize; i++)
    {
        memset(callForwarding[i].dstName, 0x00, sizeof(callForwarding[i].dstName));
        memset(callForwarding[i].srcName, 0x00, sizeof(callForwarding[i].srcName));
        strcpy(callForwarding[i].dstName, callForwarding[i + 1].dstName);
        strcpy(callForwarding[i].srcName, callForwarding[i + 1].srcName);
    }

    callForwardingSize -= 1;

    return CALL_FORWARDING_SUCCESS;
}

int CallForwarding_Add_List(char *key, char *value)
{
    if (strlen(key) >= 20 || strlen(value) >= 20 || strlen(key) == 0 || strlen(value) == 0 || key == NULL || value == NULL)
    {
        return CALL_FORWARDING_FAIL;
    }
    if (CallForwarding_Search_List(key) >= 0)
    {
        return CALL_FORWARDING_FAIL;
    }
    int toAddIdx = callForwardingSize;
    /* add Array */
    memset(callForwarding[toAddIdx].dstName, 0x00, sizeof(callForwarding[toAddIdx].dstName));
    memset(callForwarding[toAddIdx].srcName, 0x00, sizeof(callForwarding[toAddIdx].srcName));
    strcpy(callForwarding[toAddIdx].dstName, value);
    strcpy(callForwarding[toAddIdx].srcName, key);

    /* add Hash */
    unsigned int hashKey = CallForwarding_Get_Key(callForwarding[toAddIdx].srcName);
    int size = callForwardingHashTable[hashKey][0];
    callForwardingHashTable[hashKey][size + 1] = toAddIdx;
    callForwardingHashTable[hashKey][0] += 1;

    callForwardingSize++;

    return CALL_FORWARDING_SUCCESS;
}

int CallForwarding_Search_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return CALL_FORWARDING_FAIL;
    }
    unsigned int key = CallForwarding_Get_Key(name);
    int size = callForwardingHashTable[key][0];
    int i = 0;
    for (i = 1; i <= size; i++)
    {
        int hashToIdx = callForwardingHashTable[key][i];
        if (strcmp(name, callForwarding[hashToIdx].srcName) == 0)
        {
            return hashToIdx;
        }
    }
    return CALL_FORWARDING_FAIL;
}

int CallForwarding_Save_File()
{
    /* DATABASE LOADING */
    FILE *fp;
    int i = 0;

    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/CallForwardingTable.txt", "w")) == NULL)
    {
        perror("[FOPEN]:");
        return CALL_FORWARDING_FAIL;
    }

    for (i = 0; i < callForwardingSize; i++)
    {
        fprintf(fp, "%s : %s\n", callForwarding[i].srcName, callForwarding[i].dstName);
    }

    fclose(fp);

    return CALL_FORWARDING_SUCCESS;
}

unsigned int CallForwarding_Get_Key(char *name)
{
    unsigned int ret = 0;
    int toMul = 1;
    int i = 0;
    for (i = 0; name[i] != '\0'; i++)
    {
        ret += ((int)name[i] * toMul) % 100;
        toMul *= CALL_FORWARDING_PN;
    }
    return ret % 100;
}
