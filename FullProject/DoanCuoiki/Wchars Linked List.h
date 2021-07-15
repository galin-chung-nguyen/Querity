#pragma once
#include <stdio.h>
#include <string>
#include <wchar.h>
#include <iostream>

struct WcharsListNode
{
    std::wstring word;
    WcharsListNode *pNext = nullptr;
    WcharsListNode *pPrev = nullptr;
};

struct WcharsLinkedList
{
    WcharsListNode *pHead = nullptr;
    WcharsListNode *pTail = nullptr;
    int nWords = 0;
};

WcharsListNode *newWcharsListNode(std::wstring word, WcharsListNode* pPrev, WcharsListNode *pNext)
{
    WcharsListNode *p;
    p = new WcharsListNode;
    if (p)
    {
        p->word = word;
        p->pNext = pNext;
        p->pPrev = pPrev;
        if (pNext != nullptr)
            pNext->pPrev = p;
        if (pPrev != nullptr)
            pPrev->pNext = p;
    }
    return p;
}

void freeMemoryWcharsListNode(WcharsListNode *&p)
{
    if (p != nullptr)
        delete p;
    p = nullptr;
}

bool isEmptyWcharsLinkedList(WcharsLinkedList list)
{
    return list.nWords == 0 && list.pHead == nullptr && list.pTail == nullptr;
}

void showwCharsLinkedList(WcharsLinkedList list)
{
    if (isEmptyWcharsLinkedList(list))
        return;
    WcharsListNode *p = list.pHead;
    while (p != nullptr)
    {
        std::wcout << p->word << L" ";
        p = p->pNext;
    }
    wprintf(L"\n");
}



void addHeadWcharsLinkedList(std::wstring data, WcharsLinkedList &list)
{
    bool isEmptyList = isEmptyWcharsLinkedList(list);
    WcharsListNode *p = newWcharsListNode(data, nullptr, list.pHead);
    if (p != nullptr)
    {
        list.pHead = p;
        if (isEmptyList)
            list.pTail = p;
        list.nWords++;
    }
}

void addTailWcharsLinkedList(std::wstring word, WcharsLinkedList &list)
{
    if (isEmptyWcharsLinkedList(list))
        addHeadWcharsLinkedList(word, list);
    else
    {
        WcharsListNode *p = newWcharsListNode(word, list.pTail, nullptr);
        if (p == nullptr)
            return;
        list.pTail = p;
        list.nWords++;
    }
}

void deleteAllWcharsLinkedList(WcharsLinkedList &list)
{
    if (isEmptyWcharsLinkedList(list))
        return;
    WcharsListNode *p, *q;
    p = list.pHead;
    while (p != NULL)
    {
        q = p->pNext;
        freeMemoryWcharsListNode(p);
        p = q;
    }
    list.pHead = list.pTail = nullptr;
    list.nWords = 0;
}

WcharsListNode *findByDataWcharsLinkedList(std::wstring word, int &index, WcharsLinkedList list)
{
    WcharsListNode *p = list.pHead;
    index = 0;
    while (p != NULL)
    {
        if (p->word == word)
            break;
        p = p->pNext;
        index++;
    }
    return p;
}

void deleteByDataWcharsLinkedList(std::wstring word, WcharsLinkedList &list)
{
    WcharsListNode *p = list.pHead, *q = nullptr;
    while (p != nullptr)
    {
        q = p->pNext;

        if (p->word == word)
        {
            if (p->pPrev != nullptr)
                p->pPrev->pNext = p->pNext;
            if (p->pNext != nullptr)
                p->pNext->pPrev = p->pPrev;
            freeMemoryWcharsListNode(p);
        }

        p = q;
    }
}