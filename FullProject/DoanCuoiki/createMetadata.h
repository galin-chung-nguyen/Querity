#pragma once
#include "TOKENSLIST.h"

void getValidPath(char *path);

void listFile(const char *directory, const char *outputFileName, int &nFiles);

int generateIndex(const char *dataDirectory, const char *outputFileName);

int getStopwords(const char *stopwordsFileName, TOKENSLIST &stopwords);

wchar_t *readTextFile(char *filePath);

bool isStopword(wchar_t *word, TOKENSLIST stopwords);

void getTokens(wchar_t *text, TOKENSLIST &tokens, TOKENSLIST stopwords);

void getDistinctTokens(TOKENSLIST allTokens, TOKENSLIST &distinctTokens);

void getImportantTokens(TOKENSLIST allTokens, TOKENSLIST &importantTokens);

void generateMetadata(const char *dataDirName, const char *stopwordsFileName, char *indexFileName, char *metadataFileName);
