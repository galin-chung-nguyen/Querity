#pragma once
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

//*************************************************************************
//              TOKENSLIST.h
struct TOKEN
{
    wchar_t *wdata = NULL;
    char *data = NULL;
    float occurFrequency = 0.0;
    TOKEN *pNext = NULL;
};

struct TOKENSLIST
{
    TOKEN *pHead = NULL, *pTail = NULL;
    int nTokens = 0;
};

bool isEmptyTOKENSLIST(TOKENSLIST list);
TOKEN *newTOKEN(wchar_t *wdata, char *data, TOKEN *pNext);
TOKEN *addHeadTOKENSLIST(wchar_t *wdata, char *data, TOKENSLIST &list);
TOKEN *addTailTOKENSLIST(wchar_t *wdata, char *data, TOKENSLIST &list);
void freeMemoryTOKENSLIST(TOKENSLIST &list);
void showTOKENSLIST(TOKENSLIST list);
void mergeTOKENSLIST(TOKENSLIST list1, TOKENSLIST list2, TOKENSLIST &result);
void halfSplitTOKENSLIST(TOKENSLIST origin, TOKENSLIST &half1, TOKENSLIST &half2);
void mergeSortTOKENSLIST(TOKENSLIST &list);
