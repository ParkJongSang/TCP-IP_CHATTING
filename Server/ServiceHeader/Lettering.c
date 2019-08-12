#include "Lettering.h"

int Lettering_Init_List()
{
    /* DATABASE LOADING */
    char tmpStr[128];
    FILE *fp;
    letteringSize = 0;

    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/LetteringTable.txt", "r")) == NULL)
    {
        perror("[FOPEN]:");
        return LETTERING_FAIL;
    }

    memset(tmpStr, 0x00, sizeof(tmpStr));

    /* Input Array */
    while (fgets(tmpStr, sizeof(Lettering_t), fp) != NULL)
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
            strcpy(lettering[letteringSize].srcName, key);
            strcpy(lettering[letteringSize].dstName, value);
            letteringSize += 1;
            memset(tmpStr, 0x00, sizeof(tmpStr));
        }
    }

    /* Input HashTable */
    int i = 0;
    for (i = 0; i < letteringSize; i++)
    {
        unsigned int key = Lettering_Get_Key(lettering[i].srcName);
        int size = letteringHashTable[key][0];
        letteringHashTable[key][size + 1] = i;
        letteringHashTable[key][0] += 1;
    }

    /* CLOSE FILE */
    fclose(fp);
    return LETTERING_SUCCESS;
}

int Lettering_Del_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return LETTERING_FAIL;
    }

    int idx = Lettering_Search_List(name);
    int search = 0;
    int i = 0;
    unsigned int key = Lettering_Get_Key(name);
    int size = letteringHashTable[key][0];

    if (idx < 0)
    {
        return LETTERING_FAIL;
    }

    for (i = 1; i <= size; i++)
    {
        int hashToIdx = letteringHashTable[key][i];
        if (search == 0 && strcmp(name, lettering[hashToIdx].srcName) == 0)
        {
            search = 1;
        }
        if (search == 1 && i < size)
        {
            letteringHashTable[key][i] = letteringHashTable[key][i + 1];
        }
    }
    if (search == 0)
    {
        return LETTERING_FAIL;
    }
    letteringHashTable[key][0] -= 1;

    /* del in array */
    for (i = idx; i < letteringSize; i++)
    {
        memset(lettering[i].dstName, 0x00, sizeof(lettering[i].dstName));
        memset(lettering[i].srcName, 0x00, sizeof(lettering[i].srcName));
        strcpy(lettering[i].dstName, lettering[i + 1].dstName);
        strcpy(lettering[i].srcName, lettering[i + 1].srcName);
    }
    memset(lettering[letteringSize - 1].dstName, 0x00, sizeof(lettering[letteringSize - 1].dstName));
    memset(lettering[letteringSize - 1].srcName, 0x00, sizeof(lettering[letteringSize - 1].srcName));
    letteringSize -= 1;

    return LETTERING_SUCCESS;
}

int Lettering_Add_List(char *key, char *value)
{
    if (key == NULL || value == NULL || strlen(key) >= 20 || strlen(value) >= 20 || strlen(key) == 0 || strlen(value) == 0)
    {
        return LETTERING_FAIL;
    }
    if (Lettering_Search_List(key) >= 0)
    {
        Lettering_Del_List(key);
    }
    int toAddIdx = letteringSize;

    /* add Array */
    memset(lettering[toAddIdx].dstName, 0x00, sizeof(lettering[toAddIdx].dstName));
    memset(lettering[toAddIdx].srcName, 0x00, sizeof(lettering[toAddIdx].srcName));
    strcpy(lettering[toAddIdx].dstName, value);
    strcpy(lettering[toAddIdx].srcName, key);

    /* add Hash */
    unsigned int hashKey = Lettering_Get_Key(lettering[toAddIdx].srcName);
    int size = letteringHashTable[hashKey][0];
    letteringHashTable[hashKey][size + 1] = toAddIdx;
    letteringHashTable[hashKey][0] += 1;

    letteringSize++;

    return LETTERING_SUCCESS;
}

int Lettering_Search_List(char *name)
{
    if (name == NULL || strlen(name) == 0 || strlen(name) >= 20)
    {
        return LETTERING_FAIL;
    }

    unsigned int key = Lettering_Get_Key(name);
    int size = letteringHashTable[key][0];
    int i = 0;
    for (i = 1; i <= size; i++)
    {
        int hashToIdx = letteringHashTable[key][i];
        if (strcmp(name, lettering[hashToIdx].srcName) == 0)
        {
            return hashToIdx;
        }
    }

    return LETTERING_FAIL;
}

int Lettering_Save_File()
{
    /* DATABASE LOADING */
    FILE *fp;
    int i = 0;

    if ((fp = fopen("/mnt/c/Users/User/Desktop/TermProject/Server/ServiceData/LetteringTable.txt", "w")) == NULL)
    {
        perror("[FOPEN]:");
        return LETTERING_FAIL;
    }

    for (i = 0; i < letteringSize; i++)
    {
        fprintf(fp, "%s : %s\n", lettering[i].srcName, lettering[i].dstName);
    }

    fclose(fp);

    return LETTERING_SUCCESS;
}
unsigned int Lettering_Get_Key(char *name)
{
    unsigned int ret = 0;
    int toMul = 1;
    int i = 0;
    for (i = 0; name[i] != '\0'; i++)
    {
        ret += ((int)name[i] * toMul) % 100;
        toMul *= LETTERINT_PN;
    }
    return ret % 100;
}