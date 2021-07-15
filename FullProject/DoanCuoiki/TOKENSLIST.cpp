#include "stdafx.h"

#include "TOKENSLIST.h"

//*************************************************************************
//              TOKENSLIST.cpp

bool isEmptyTOKENSLIST(TOKENSLIST list)
{
    return list.nTokens == 0;
}

TOKEN *newTOKEN(wchar_t *wdata, char *data, TOKEN *pNext)
{
    TOKEN *newToken = (TOKEN *)malloc(sizeof(TOKEN));
    if (newToken)
    {
        if (data)
        {
            newToken->data = (char *)malloc(sizeof(char) * (strlen(data) + 1));
            strcpy(newToken->data, data);
        }
        else
            newToken->data = NULL;
        if (wdata)
        {
            newToken->wdata = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(wdata) + 1));
            wcscpy(newToken->wdata, wdata);
        }
        else
            newToken->wdata = NULL;
        newToken->pNext = pNext;
    }
    return newToken;
}

TOKEN *addHeadTOKENSLIST(wchar_t *wdata, char *data, TOKENSLIST &list)
{
    TOKEN *newHeadNode = newTOKEN(wdata, data, list.pHead);
    if (newHeadNode)
    {
        if (isEmptyTOKENSLIST(list))
            list.pTail = newHeadNode;
        list.pHead = newHeadNode;
        list.nTokens++;
    }
    return newHeadNode;
}

TOKEN *addTailTOKENSLIST(wchar_t *wdata, char *data, TOKENSLIST &list)
{
    if (isEmptyTOKENSLIST(list))
        return addHeadTOKENSLIST(wdata, data, list);
    TOKEN *newTailNode = newTOKEN(wdata, data, (TOKEN *)NULL);
    list.pTail->pNext = newTailNode;
    list.pTail = newTailNode;
    list.nTokens++;
    return newTailNode;
}

void freeMemoryTOKENSLIST(TOKENSLIST &list)
{
    TOKEN *p = list.pHead;
    TOKEN *q = NULL;
    while (p)
    {
        q = p->pNext;
        if (p->data)
        {
            free(p->data);
            p->data = NULL;
        }
        if (p->wdata)
        {
            free(p->wdata);
            p->wdata = NULL;
        }
        free(p);
        p = q;
    }
    list.pHead = list.pTail = NULL;
    list.nTokens = 0;
}

void showTOKENSLIST(TOKENSLIST list)
{
    for (TOKEN *p = list.pHead; p; p = p->pNext)
        if (p->data)
            printf("%s\t", p->data);
    printf("\n");
}

// Merge 2 tokens list theo chiều giảm dần tần suất xuất hiện token
void mergeTOKENSLIST(TOKENSLIST list1, TOKENSLIST list2, TOKENSLIST &result)
{
    freeMemoryTOKENSLIST(result);
    TOKEN *p = list1.pHead, *q = list2.pHead;
    while (p && q)
    {
        if (p->occurFrequency < q->occurFrequency)
        {
            addTailTOKENSLIST(q->wdata, q->data, result);
            result.pTail->occurFrequency = q->occurFrequency;
            q = q->pNext;
        }
        else
        {
            addTailTOKENSLIST(p->wdata, p->data, result);
            result.pTail->occurFrequency = p->occurFrequency;
            p = p->pNext;
        }
    }
    while (p)
    {
        addTailTOKENSLIST(p->wdata, p->data, result);
        result.pTail->occurFrequency = p->occurFrequency;
        p = p->pNext;
    }
    while (q)
    {
        addTailTOKENSLIST(q->wdata, q->data, result);
        result.pTail->occurFrequency = q->occurFrequency;
        q = q->pNext;
    }
}

void halfSplitTOKENSLIST(TOKENSLIST origin, TOKENSLIST &half1, TOKENSLIST &half2)
{
    freeMemoryTOKENSLIST(half1);
    freeMemoryTOKENSLIST(half2);
    TOKEN *p = origin.pHead;
    int index = 0;
    int halfSize1 = origin.nTokens / 2;
    int halfSize2 = origin.nTokens - halfSize1;
    while (index < halfSize1 && p)
    {
        addTailTOKENSLIST(p->wdata, p->data, half1);
        half1.pTail->occurFrequency = p->occurFrequency;
        p = p->pNext;
        index++;
    }
    index = 0;
    while (index < halfSize2 && p)
    {
        addTailTOKENSLIST(p->wdata, p->data, half2);
        half2.pTail->occurFrequency = p->occurFrequency;
        p = p->pNext;
        index++;
    }
}

void mergeSortTOKENSLIST(TOKENSLIST &list)
{
    if (list.nTokens <= 1)
        return;
    TOKENSLIST half1, half2;
    halfSplitTOKENSLIST(list, half1, half2);
    mergeSortTOKENSLIST(half1);
    mergeSortTOKENSLIST(half2);
    mergeTOKENSLIST(half1, half2, list);
    freeMemoryTOKENSLIST(half1);
    freeMemoryTOKENSLIST(half2);
}

int cmpTwoTokens(TOKEN *a, TOKEN *b){
	return strcmp(a->data, b->data);
}

//****************************************************************************