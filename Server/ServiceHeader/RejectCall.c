#include "RejectCall.h"

int RejectCall_Init_List()
{
    /* DATABASE LOADING */
    char tmpStr[2048];
    FILE *fp;
    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/RejectCallTable.txt", "r")) == NULL)
    {
        perror("[FOPEN]:");
        return REJECT_CALL_FAIL;
    }

    memset(tmpStr, 0x00, sizeof(tmpStr));

    /* Input Array */
    while (fgets(tmpStr, sizeof(RejectCall_t), fp) != NULL)
    {
        rejectCall[rejectCallSize].dstNameSize = 0;
        int len = strlen(tmpStr);
        int i = 0;
        int key_value = 0;
        char key[20], keyIdx = 0;
        char value[100][20], valueIdx = 0;

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
                rejectCall[rejectCallSize].dstNameSize += 1;
                break;
            }
            if (tmpStr[i] == ',')
            {
                valueIdx = 0;
                rejectCall[rejectCallSize].dstNameSize += 1;
                continue;
            }
            if (key_value == 0)
            {
                key[keyIdx++] = tmpStr[i];
            }
            else
            {
                value[rejectCall[rejectCallSize].dstNameSize][valueIdx++] = tmpStr[i];
            }
        }

        if(strlen(key) > 0)
        {
            strcpy(rejectCall[rejectCallSize].srcName, key);
            for (i = 0; i < rejectCall[rejectCallSize].dstNameSize; i++)
            {
                if(strlen(value[i]) > 0){
                    strcpy(rejectCall[rejectCallSize].dstName[i], value[i]);
                }
            }
            rejectCallSize += 1;
            memset(tmpStr, 0x00, sizeof(tmpStr));
        }
    }

    int i = 0;
    for (i = 0; i < rejectCallSize; i++)
    {
        unsigned int key = RejectCall_Get_Key(rejectCall[i].srcName);
        int size = rejectCallHashTable[key][0];
        rejectCallHashTable[key][size + 1] = i;
        rejectCallHashTable[key][0] += 1;
    }

    /* CLOSE FILE */
    fclose(fp);
    return REJECT_CALL_SUCCESS;
}

int RejectCall_Del_List(char *key, char *value)
{
    if (key == NULL || value == NULL || strlen(key) == 0 || strlen(key) >= 20 || strlen(value) == 0 || strlen(value) >= 20)
    {
        return REJECT_CALL_FAIL;
    }

    int i = 0;
    int search = 0;
    /* del in hashTable*/
    int keyIdx = RejectCall_Search_Key_List(key);
    int valueIdx = RejectCall_Search_Value_List(key, value);
    if (keyIdx < 0 || valueIdx < 0)
    {
        return REJECT_CALL_FAIL;
    }
    int valueSize = rejectCall[keyIdx].dstNameSize;
    unsigned int hashKey = RejectCall_Get_Key(key);
    int size = rejectCallHashTable[hashKey][0];

    if (keyIdx < 0 || valueIdx < 0)
    {
        return REJECT_CALL_FAIL;
    }

    /* Delete one of Values */
    for (i = valueIdx; i < valueSize; i++)
    {
        memset(rejectCall[keyIdx].dstName[i], 0x00, sizeof(rejectCall[keyIdx].dstName[i]));
        strcpy(rejectCall[keyIdx].dstName[i], rejectCall[keyIdx].dstName[i + 1]);
    }

    memset(rejectCall[keyIdx].dstName[valueSize - 1], 0x00, sizeof(rejectCall[keyIdx].dstName[valueSize - 1]));
    rejectCall[keyIdx].dstNameSize -= 1;

    /* Delete on Hash when ValueSize = 0 */

    if (rejectCall[keyIdx].dstNameSize == 0)
    {
        /* DEL HASHTABLE */
        for (i = 1; i <= size; i++)
        {
            int hashToIdx = rejectCallHashTable[hashKey][i];
            if (search == 0 && strcmp(key, rejectCall[hashToIdx].srcName) == 0)
            {
                search = 1;
            }
            if (search == 1 && i < size)
            {
                rejectCallHashTable[hashKey][i] = rejectCallHashTable[hashKey][i + 1];
            }
        }
        if (search == 0)
        {
            return REJECT_CALL_FAIL;
        }
        rejectCallHashTable[hashKey][0] -= 1;

        /* DEL ARRAY */
        for (i = keyIdx; i < rejectCallSize; i++)
        {
            int j = 0;
            int dstNameSize = rejectCall[i].dstNameSize;
            for (j = 0; j < dstNameSize; j++)
            {
                memset(rejectCall[i].dstName[j], 0x00, sizeof(rejectCall[i].dstName[j]));
                strcpy(rejectCall[i].dstName[j], rejectCall[i + 1].dstName[j]);
            }
            memset(rejectCall[i].srcName, 0x00, sizeof(rejectCall[i].srcName));
            strcpy(rejectCall[i].srcName, rejectCall[i + 1].srcName);
        }
        rejectCallSize--;
    }

    return REJECT_CALL_SUCCESS;
}

int RejectCall_Add_List(char *key, char *value)
{
    if (key == NULL || value == NULL || strlen(key) >= 20 || strlen(value) >= 20 || strlen(key) == 0 || strlen(value) == 0)
    {
        return REJECT_CALL_FAIL;
    }
    int keyIdx = RejectCall_Search_Key_List(key);
    int valueIdx = RejectCall_Search_Value_List(key, value);

    if (keyIdx >= 0 && valueIdx >= 0)
    {
        return REJECT_CALL_FAIL;
    }
    else if (keyIdx >= 0 && valueIdx < 0)
    {
        int toAddIdx = keyIdx;
        int valueSize = rejectCall[toAddIdx].dstNameSize;
        memset(rejectCall[toAddIdx].dstName[valueSize], 0x00, sizeof(rejectCall[toAddIdx].dstName[valueSize]));
        memset(rejectCall[toAddIdx].srcName, 0x00, sizeof(rejectCall[toAddIdx].srcName));
        strcpy(rejectCall[toAddIdx].dstName[valueSize], value);
        strcpy(rejectCall[toAddIdx].srcName, key);
        rejectCall[keyIdx].dstNameSize++;
    }
    else if (keyIdx < 0 && valueIdx < 0)
    {
        int toAddKeyIdx = rejectCallSize;
        int toAddValueIdx = rejectCall[toAddKeyIdx].dstNameSize;
        memset(rejectCall[toAddKeyIdx].dstName[toAddValueIdx], 0x00, sizeof(rejectCall[toAddKeyIdx].dstName[toAddValueIdx]));
        memset(rejectCall[toAddKeyIdx].srcName, 0x00, sizeof(rejectCall[toAddKeyIdx].srcName));
        strcpy(rejectCall[toAddKeyIdx].dstName[toAddValueIdx], value);
        strcpy(rejectCall[toAddKeyIdx].srcName, key);

        rejectCall[toAddKeyIdx].dstNameSize++;
        rejectCallSize++;

        unsigned int hashKey = RejectCall_Get_Key(rejectCall[toAddKeyIdx].srcName);
        int size = rejectCallHashTable[hashKey][0];
        rejectCallHashTable[hashKey][size + 1] = toAddKeyIdx;
        rejectCallHashTable[hashKey][0] += 1;
    }
    else
    {
        return REJECT_CALL_FAIL;
    }

    return REJECT_CALL_SUCCESS;
}

int RejectCall_Search_Key_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return REJECT_CALL_FAIL;
    }
    unsigned int key = RejectCall_Get_Key(name);
    int size = rejectCallHashTable[key][0];
    int i = 0;
    for (i = 1; i <= size; i++)
    {
        int hashToIdx = rejectCallHashTable[key][i];
        if (strcmp(name, rejectCall[hashToIdx].srcName) == 0)
        {
            return hashToIdx;
        }
    }
    return REJECT_CALL_FAIL;
}

int RejectCall_Search_Value_List(char *key, char *value)
{
    if (key == NULL || value == NULL || strlen(key) == 0 || strlen(key) >= 20 || strlen(value) == 0 || strlen(value) >= 20)
    {
        return REJECT_CALL_FAIL;
    }
    int keyIdx = RejectCall_Search_Key_List(key);
    int valueSize = rejectCall[keyIdx].dstNameSize;
    int i = 0;
    if (keyIdx < 0)
    {
        return REJECT_CALL_FAIL;
    }

    for (i = 0; i < valueSize; i++)
    {
        if (i == valueSize - 1)
        {
            size_t destNameLen = strlen(rejectCall[keyIdx].dstName[i]) - 1;
            if (strncmp(value, rejectCall[keyIdx].dstName[i], destNameLen) == 0)
            {
                return i;
            }
        }
        else
        {
            if (strcmp(value, rejectCall[keyIdx].dstName[i]) == 0)
            {
                return i;
            }
        }
    }
    return REJECT_CALL_FAIL;
}

int RejectCall_Save_File()
{
    /* DATABASE LOADING */
    FILE *fp;
    int i = 0;
    int j = 0;
    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/RejectCallTable.txt", "w")) == NULL)
    {
        perror("[FOPEN]:");
        return REJECT_CALL_FAIL;
    }

    for (i = 0; i < rejectCallSize; i++)
    {
        j = 0;
        fprintf(fp, "%s : %s", rejectCall[i].srcName, rejectCall[i].dstName[j]);
        for (j = 1; j < rejectCall[i].dstNameSize; j++)
        {
            fprintf(fp, ", %s", rejectCall[i].dstName[j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    return REJECT_CALL_SUCCESS;
}

unsigned int RejectCall_Get_Key(char *name)
{
    unsigned int ret = 0;
    int toMul = 1;
    int i = 0;
    for (i = 0; name[i] != '\0'; i++)
    {
        ret += ((int)name[i] * toMul) % 100;
        toMul *= REJECT_CALL_PN;
    }
    return ret % 100;
}

void printReject()
{
    int i = 0, j = 0;
    for (i = 0; i < rejectCallSize; i++)
    {
        printf("%s :", rejectCall[i].srcName);
        for (j = 0; j < rejectCall[i].dstNameSize; j++)
        {
            printf(" %s", rejectCall[i].dstName[j]);
        }
        printf("\n");
    }
}