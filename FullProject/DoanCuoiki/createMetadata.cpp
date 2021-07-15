#include "stdafx.h"

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <iostream>
#include "dirent.h"
#include "TOKENSLIST.h"
#include "createMetadata.h"
#include "filePath.h"

using namespace std;

//*************************************************************************
//              TOKENSLIST.h
/*struct TOKEN
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
}*/

//****************************************************************************

void getValidPath(char* path)
{
    int len = strlen(path);
    for (int i = 0; i < len; i++)
    {
        if (path[i] == '\\')
            path[i] = '/';
    }
}

void listFile(const char *directory, const char *outputFileName, int &nFiles)
{
    DIR *dir = opendir(directory);

    if (dir == NULL)
        return;

    struct dirent *intity = readdir(dir);

    FILE *output = fopen(outputFileName, "a");

    while (intity)
    {
        if (intity->d_type != DT_DIR)
        {
            fprintf(output, "add|%s/%s\n", directory, intity->d_name);
            nFiles++;
        }

        else if (strcmp(intity->d_name, ".") != 0 && strcmp(intity->d_name, "..") != 0)
        {
            char path[500] = {'\0'};
            strcat(path, directory);
            strcat(path, "/");
            strcat(path, intity->d_name);
            listFile(path, outputFileName, nFiles);
        }

        intity = readdir(dir);
    }

    fclose(output);
    closedir(dir);
}

int generateIndex(const char *dataDirectory, const char *outputFileName)
{
	wprintf(L"****************************************\nGenerating index.txt... ");
	FILE *pOut = fopen(outputFileName, "w");
	fclose(pOut);

	// Lấy thư mục hiện hành và gán thêm thư mục data vào sau
	char currentDirectory[1000];
	GetFullPathName((LPCWSTR)outputFileName, 1000, (LPWSTR)currentDirectory, nullptr);

	int index = strlen(currentDirectory) - 1;
	while (currentDirectory[index] != '\\' && currentDirectory[index] != '/')
		index--;
	currentDirectory[index + 1] = '\0';
	strcat(currentDirectory, dataDirectory);
	getValidPath(currentDirectory);

    int nFiles = 0;
    listFile(currentDirectory, outputFileName, nFiles);
    wprintf(L"DONE!\nNumber of files: %d files\n****************************************\n", nFiles);
    return nFiles;
}

// Hàm đọc các stopwords, trả về số lượng stopwords
int getStopwords(const char *stopwordsFileName, TOKENSLIST &stopwords)
{
    freeMemoryTOKENSLIST(stopwords);

    FILE *pFileIn = fopen(stopwordsFileName, "r,ccs=UTF-16LE");

    if (pFileIn == NULL)
    {
        wprintf(L"\nERROR! %s NOT FOUND!\n", stopwordsFileName);
        return -1;
    }

    wchar_t stopword[50];

    while (fgetws(stopword, 50, pFileIn))
    {
        stopword[wcslen(stopword) - 1] = L'\0';
        addHeadTOKENSLIST(stopword, NULL, stopwords);
    }

    fclose(pFileIn);

    return stopwords.nTokens;
}

wchar_t *readTextFile(char *filePath)
{
    FILE *textStream = fopen(filePath, "r,ccs=UTF-16LE");

    if (textStream == NULL)
    {
        wprintf(L"\nERROR! %s NOT FOUND!\n", filePath);
        return NULL;
    }

    // Đếm số kí tự
    int nChars = 0;
    wchar_t wc;
	wstring txt = L"";
	while ((wc = fgetwc(textStream)) != WEOF) {//while (fwscanf(textStream, L"%lc", &wc))
		nChars++;
		txt += wc;
	}

    // Đọc nội dung lưu vào mảng kí tự
    wchar_t *content = (wchar_t *)malloc(sizeof(wchar_t) * (nChars + 1));
	for (int i = 0; i < nChars; ++i) content[i] = txt[i];
    content[nChars] = L'\0';

    fclose(textStream);

    return content;
}

bool isStopword(wchar_t *word, TOKENSLIST stopwords)
{
    for (TOKEN *p = stopwords.pHead; p; p = p->pNext)
    {
        if (wcscmp(word, p->wdata) == 0)
            return true;
    }
    return false;
}

// Hàm tách chuỗi tiếng việt có dấu thành list các token không dấu, loại bỏ stopword
void getTokens(wchar_t *text, TOKENSLIST &tokens, TOKENSLIST stopwords)
{
    freeMemoryTOKENSLIST(tokens);

    const wchar_t *delim = L" `~!@#$%^&*()_–-+=/“[]{}\\|;:'\"<>,.?/\n\t";

    wchar_t *token = wcstok(text, delim);

    while (token)
    {
        if (isStopword(token, stopwords) == false)
        {
            char *noAccentData = NULL;
            standardize(token, noAccentData);
            addTailTOKENSLIST(token, noAccentData, tokens);
            free(noAccentData);
            noAccentData = NULL;
        }
        token = wcstok(NULL, delim);
    }
}

// Hàm lấy các token không lặp lại (không dấu)
void getDistinctTokens(TOKENSLIST allTokens, TOKENSLIST &distinctTokens)
{
    freeMemoryTOKENSLIST(distinctTokens);
    for (TOKEN *p = allTokens.pHead; p; p = p->pNext)
    {
        bool found = false;
        for (TOKEN *q = distinctTokens.pHead; q; q = q->pNext)
        {
            if (strcmp(p->data, q->data) == 0)
            {
                found = true;
                break;
            }
        }
        if (found == false)
            addTailTOKENSLIST(p->wdata, p->data, distinctTokens);
    }
}

// Hàm lấy các từ quan trọng dựa trên tần suất xuất hiện
void getImportantTokens(TOKENSLIST allTokens, TOKENSLIST &importantTokens)
{
    freeMemoryTOKENSLIST(importantTokens);

    TOKENSLIST distinctTokens;
    getDistinctTokens(allTokens, distinctTokens);

    // Duyệt qua tất cả các token không lặp lại
    for (TOKEN *p = distinctTokens.pHead; p; p = p->pNext)
    {
        // Đếm số lần xuất hiện của token hiện tại trong allTokens
        int count = 0;
        for (TOKEN *q = allTokens.pHead; q; q = q->pNext)
        {
            if (strcmp(q->data, p->data) == 0)
                count++;
        }

        // Tính tần suất xuất hiện của token hiện tại (f)
        float f = (float)count / (float)allTokens.nTokens;

        // Nếu f >= 0.015 thì token đó là quan trọng
        if (f >= 0.015)
        {
            //printf("\nword: %s\tcount: %d\t total: %d\n", p->data, count, allTokens.nTokens);
            addTailTOKENSLIST(p->wdata, p->data, importantTokens);
            importantTokens.pTail->occurFrequency = f;
        }
    }

    freeMemoryTOKENSLIST(distinctTokens);
}

void generateMetadata(const char *dataDirName, const char *stopwordsFileName, char *indexFileName, char *metadataFileName)
{
    // Tạo tệp tin index.txt
	int nFiles = generateIndex(dataDirName, indexFileName);

	wcout << "Generating " << metadataFileName << "...";
    clock_t startTime = clock();

    // Đọc file stopwords
    TOKENSLIST stopwords;
    int nStopwords = getStopwords(stopwordsFileName, stopwords);

    if (nStopwords == -1)
        return;

    // Đọc file index.txt, xử lý lần lượt từng tệp văn bản
    FILE *indexFile = fopen(indexFileName, "r");

    char path[1000]; // Đường dẫn của một tệp văn bản
    char state[20]; // Trạng thái của văn bản (add hoặc delete)
	char tmp;

    FILE *metadata = fopen(metadataFileName, "w"); // Tệp tin siêu dữ liệu

    // Đọc từng file văn bản dựa trên path
    int initializedFiles = 0;
    while (initializedFiles < nFiles)
    {
        fscanf(indexFile, "%[^|]", &state);
		fscanf(indexFile, "%c",&tmp);
        fgets(path, 1000, indexFile);

		if(path[strlen(path) - 1]) path[strlen(path) - 1] = '\0';
		
		// Đọc nội dung file văn bản
        wchar_t *text = readTextFile(path);
        if (!text) break;

        // Tách ra các tokens (đồng thời xóa dấu và lọc stopword)
        TOKENSLIST allTokens;
        getTokens(text, allTokens, stopwords);

        // Lấy các token quan trọng
        TOKENSLIST importantTokens;
        getImportantTokens(allTokens, importantTokens);

        // Sắp xếp importantTokens theo chiều giảm dần tần suất xuất hiện của tokens
        mergeSortTOKENSLIST(importantTokens);

        // Ghi kết quả vào tệp tin siêu dữ liệu
        fprintf(metadata, "%s\n", path);
        for (TOKEN *p = importantTokens.pHead; p; p = p->pNext)
            fprintf(metadata, "%s[%.5f] ", p->data, p->occurFrequency);
        fprintf(metadata, "\n");

        free(text);
        freeMemoryTOKENSLIST(allTokens);
        freeMemoryTOKENSLIST(importantTokens);
        text = NULL;

        initializedFiles++;
    }

    fclose(indexFile);
    fclose(metadata);
    freeMemoryTOKENSLIST(stopwords);
	wcout << "DONE!\n";
    wprintf(L"Time used: %lfs\n****************************************\n", (double)((clock() - startTime) / CLOCKS_PER_SEC));
}

/*int main(int argc, char *argv[])
{
    // Command line: createMetadata.exe <data directory> <stopword file> <name of index file> <name of metadata>
    // Example: createMetadata.exe ./train ./stopword_one_word.txt index.txt metadata.txt
    if (argc != 5)
    {
        printf("COMMAND ERROR!\nUses: createMetadata.exe <data directory> <stopword file> <name of index file> <name of metadata file>");
        return -1;
    }
    else
        generateMetadata(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}*/