#include "db.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static char g_szFileName[1024];

// Установка імені файла з словником.
void DB_SetFileName(const char* szFileName)
{
    if (szFileName != NULL && szFileName[0] != '\0' && lstrlen(szFileName) < 1024)
    {
        memset(g_szFileName, 0, 1024);
        lstrcpy(g_szFileName, szFileName);
    }
}

// Додання слів у словник.
BOOL DB_AddWords(const char* szUkrainian, const char* szEnglish)
{
    FILE* file = fopen(g_szFileName, "a+");
    if (file == NULL)
    {
        return FALSE;
    }
    
    char data[2048];
    memset(data, 0, 2048);
    wsprintf(data, "[%s][%s]\n", szUkrainian, szEnglish);
    
    fwrite(data, 1, lstrlen(data), file);
    
    fclose(file);
    
    return TRUE;
}

// Переклад.
BOOL DB_Translate(const char* szRequest, char* szResult, int mode)
{
    if (szRequest == NULL || szResult == NULL)
    {
        return FALSE;
    }
    
    // Відкриваєм і зчитуєм файл.
    FILE* file = fopen(g_szFileName, "rw");
    if (file == NULL)
    {
        return FALSE;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize <= 0)
    {
        fclose(file);
        return FALSE;
    }
    
    char* buff = (char*)malloc(fileSize + 1);
    if (buff == NULL)
    {
        fclose(file);
        return FALSE;
    }
    memset(buff, 0, fileSize + 1);
    
    fread(buff, 1, fileSize, file);
    fclose(file);

    buff[fileSize] = 0;
    
    
    int i, j;
    
    int k = 0;
    
    char szWord[256];
    memset(szWord, 0, 256);
    
    for (i = 0; i < lstrlen(szRequest); i++)
    {
        if (szRequest[i] == ' ' || i == lstrlen(szRequest) - 1)
        {
            if (i == lstrlen(szRequest) - 1 && szRequest[i] != ' ')
            {
                szWord[k++] = szRequest[i];
            }
            
            int k1 = 0;
            int k2 = 0;
            
            char szUkrainian[64];
            memset(szUkrainian, 0, 64);
            
            char szEnglish[64];
            memset(szEnglish, 0, 64);
            
            int sw = 0;
            
            int found = 0;
            
            for (j = 0; j < lstrlen(buff); j++)
            {
                if (buff[j] == '[' && sw == 0)
                {
                    sw = 1;
                    continue;
                }
                
                if (buff[j] == ']' && sw == 1)
                {
                    sw = 2;
                    continue;
                }
                
                if (buff[j] == '[' && sw == 2)
                {
                    sw = 3;
                    continue;
                }
                
                if (buff[j] == ']' && sw == 3)
                {
                    sw = 4;
                    continue;
                }
                
                if (sw == 0 || sw == 2)
                {
                    continue;
                }
                
                if (sw == 1)
                {
                    szUkrainian[k1++] = buff[j];
                    continue;
                }
                
                if (sw == 3)
                {
                    szEnglish[k2++] = buff[j];
                    continue;
                }
                
                if (sw == 4)
                {
                    if (mode == UKRAINIAN_TO_ENGLISH)
                    {
                        if (!lstrcmp(szWord, szUkrainian))
                        {
                            lstrcat(szResult, szEnglish);
                            lstrcat(szResult, " ");
                            found = 1;
                            break;
                        }
                    }
                    else if (mode == ENGLISH_TO_UKRAINIAN)
                    {
                        if (!lstrcmp(szWord, szEnglish))
                        {
                            lstrcat(szResult, szUkrainian);
                            lstrcat(szResult, " ");
                            found = 1;
                            break;
                        }
                    }
                    
                    k1 = 0;
                    k2 = 0;
                    
                    memset(szUkrainian, 0, 64);
                    memset(szEnglish, 0, 64);
                    
                    sw = 0;
                    
                    continue;
                }
            }
            
            if (found == 0)
            {
                lstrcat(szResult, "[");
                lstrcat(szResult, szWord);
                lstrcat(szResult, "]");
                lstrcat(szResult, " ");
            }
            
            k = 0;
            memset(szWord, 0, 256);
        }
        else
        {
            if (k < 256)
            {
                szWord[k++] = szRequest[i];
            }
        }
    }
    
    return TRUE;
}

