#ifdef _MSC_VER
   #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AiStrAppender.h"

#define STR_GROW_SIZE 512

int AiStrAppenderInit(AiStrAppender *strAppender)
{
   if(!strAppender)
      return 0;
   strAppender->allocSize = STR_GROW_SIZE;
   strAppender->strSize   = 0;
   strAppender->str       = (char *) malloc(strAppender->allocSize + 1);
   *strAppender->str      = 0;
   if(!strAppender->str)
      return 0;
   return 1;
}

int AiStrAppenderWorkChar(AiStrAppender *strAppender, char toAppend)
{
   char str[2];
   str[0] = toAppend;
   str[1] = 0;
   return AiStrAppenderWork(strAppender, str);
}

int AiStrAppenderWork(AiStrAppender *strAppender, char *toAppend)
{
   size_t toAppendSize = strlen(toAppend);
   if(toAppendSize == 0)
      return 1;
   if(!strAppender || !toAppend)
      return 0;
   strAppender->strSize += toAppendSize;
   if(strAppender->strSize > strAppender->allocSize)
   {
      void *mem;
      strAppender->allocSize += STR_GROW_SIZE;
      mem = realloc(strAppender->str, strAppender->allocSize + 1);
      if(!mem)
         return 0;
      strAppender->str = (char *) mem; 
   }
   strcat(strAppender->str, toAppend); 
   return 1; 
}