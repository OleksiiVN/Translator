#ifndef _DB_H_
#define _DB_H_

#include <windows.h>

void DB_SetFileName(const char* szFileName);

BOOL DB_AddWords(const char* szUkrainian, const char* szEnglish);

#define UKRAINIAN_TO_ENGLISH 0x01
#define ENGLISH_TO_UKRAINIAN 0x02

BOOL DB_Translate(const char* szRequest, char* szResult, int mode);

#endif

