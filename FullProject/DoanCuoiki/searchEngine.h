#pragma once
#include "stdint.h"
#include <algorithm>

using namespace std;

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

struct TextFile{
	string filePath, fileTitle;
	TOKENSLIST tokenList;
	TextFile* nextFile;
	bool used;

	TextFile(string path = "", string title = "", TextFile* next = NULL) {
		filePath = path;
		fileTitle = title;
		nextFile = next;
		used = true;
	}
};

struct DataContainer{
	TextFile *textData;

	TextFile* newFile(const char* filePath);
	TextFile* fileExist(const char* filePath);

	void freeMem() {
		auto ptr = textData;
		while (ptr) {
			freeMemoryTOKENSLIST(ptr->tokenList);
			auto ptr2 = ptr->nextFile;
			delete ptr;
			ptr = ptr2;
		}
		textData = NULL;
		//fileMap.clear();
	}

	DataContainer() {
		textData = NULL;
	}
};
struct searchEngineObject {
	static constexpr double alpha = 0.015;
	static pair<double, TextFile*>* assessRelevance(TextFile* textData, TOKENSLIST tokens, int &cntRes);
	static bool isStopWord(wstring token, TOKENSLIST &stopWords);
	static void search(TextFile* textData, wchar_t rawSearchQuery[], TOKENSLIST &stopWords);
};

// Data Conatainer definitions
TextFile*  DataContainer::fileExist(const char* filePath) { // check if this file has ever bene added into the data container
	string pathStr = toLowerCase(normalizePath((string)filePath));

	auto tmp = DataContainer::textData;
	while (tmp) {
		if (strcmp(pathStr.c_str(), toLowerCase(tmp->filePath.c_str()).c_str()) == 0) {
			return tmp;
		}
		tmp = tmp->nextFile;
	}
	return NULL;
}

TextFile* DataContainer::newFile(const char* filePath) {
	string pathStr = toLowerCase(normalizePath((string)filePath));
	
	auto tmp = DataContainer::textData;
	while (tmp) {
		if (strcmp(pathStr.c_str(), toLowerCase(tmp->filePath.c_str()).c_str()) == 0) {
			return tmp;
		}
		tmp = tmp->nextFile;
	}

	auto res = new TextFile(pathStr,"Just some random title", DataContainer::textData);
	DataContainer::textData = res;
	return res;
}


// Search engine object definitions
pair<double, TextFile*>* searchEngineObject::assessRelevance(TextFile* textData, TOKENSLIST tokens, int &cntRes) {
	int cntFiles = 0;
	auto textFilePtr = textData;
	while (textFilePtr) {
		if (textFilePtr->used) {
			++cntFiles;
			//wcout << textFilePtr->filePath.c_str() << "\n";
		}
		textFilePtr = textFilePtr->nextFile;
	}
	
	// assess relevance of each text file

	pair<double, TextFile*> *res = new pair<double, TextFile*>[cntFiles];

	textFilePtr = textData;
	for (int i = 0; i < cntFiles; ++i) { // for each text file
		while(!textFilePtr->used) textFilePtr = textFilePtr->nextFile;
		res[i] = make_pair(0, textFilePtr);

		auto tokenPtr = tokens.pHead;
		while(tokenPtr){ // iterate over each keyword

			auto fileTokenPtr = textFilePtr->tokenList.pHead;
			while (fileTokenPtr) { // find the occurFrequency of current token in current file

				if (cmpTwoTokens(fileTokenPtr, tokenPtr) == 0) {
					res[i].first += fileTokenPtr->occurFrequency;
					break;
				}

				fileTokenPtr = fileTokenPtr->pNext;
			}

			tokenPtr = tokenPtr->pNext;
		}
		textFilePtr = textFilePtr->nextFile;
	}
	/*wcout << "List files :\n";
	for (int i = 0; i < cntFiles; ++i) {
		wcout << res[i].second->filePath.c_str() << " " << res[i].first << "\n";
	}
	wcout << "\n";*/

	sort(res, res + cntFiles, [&](pair<double, TextFile*> a, pair<double, TextFile*> b) {
		return a.first > b.first; // sort by relevance
	});

	cntRes = cntFiles;

	return res;
}

string toString(wstring S) {
	string res = "";
	for (int i = 0; i < S.size(); ++i) res += (char)S[i];
	return res;
}

bool searchEngineObject::isStopWord(wstring token, TOKENSLIST &stopWords){
	auto tmp = stopWords.pHead;
	while (tmp) {
		if (wstring(tmp->wdata) == token) return true;
		tmp = tmp->pNext;
	}
	return false;
}

void searchEngineObject::search(TextFile* textData, wchar_t rawSearchQuery[], TOKENSLIST &stopWords) {

	clock_t startTime = clock();

	wstring searchQueryStr(rawSearchQuery);

	const wchar_t *upcase = L"ÁÀẢÃẠĂẮẰẲẴẶÂẤẦẨẪẬĐÊẾỀỂỄỆÍÌỈĨỊÓÒỎÕỌÔỐỒỔỖỘƠỚỜỞỠỢÚÙỦŨỤƯỨỪỬỮỰÝỲỶỸỴ";
	const wchar_t *lowcase = L"áàảãạăắằẳẵặâấầẩẫậđêếềểễệíìỉĩịóòỏõọôốồổỗộơớờởỡợúùủũụưứừửữựýỳỷỹỵ";
	const wchar_t *unmarked = L"aaaaaaaaaaaaaaaaadeeeeeeiiiiiooooooooooooooooouuuuuuuuuuuyyyyy";

	for (int i = 0; i < wcslen(rawSearchQuery); ++i) {
		for (int j = 0; j < wcslen(upcase); ++j) {
			if (rawSearchQuery[i] == upcase[j]) rawSearchQuery[i] = lowcase[j];
		}
	}
	// now split the raw search query into tokens
	wchar_t* ptr;
	TOKENSLIST tokenList;
	const wchar_t *delim = L" `~!@#$%^&*()_–-+=/“[]{}\\|;:'\"<>,.?/\n\t"; // the delimeter of the string

	wchar_t* token = wcstok(rawSearchQuery, delim, &ptr);
	while (token) {
		// split tokens in this word chain]
		wstring tokenStr(token);
		if (!searchEngineObject::isStopWord(wstring(token), stopWords)) {
			for (int i = 0; i < tokenStr.size(); ++i)
				for (int j = 0; j < wcslen(unmarked); ++j) if (tokenStr[i] == lowcase[j]) tokenStr[i] = unmarked[j];

			addHeadTOKENSLIST((wchar_t *)tokenStr.c_str(), (char*)toString(tokenStr).c_str(), tokenList);
		}
		token = wcstok(NULL, delim, &ptr);
	}

	int n;

	auto res = searchEngineObject::assessRelevance(textData, tokenList,n);

	wprintf(L"\n### Search results for '%s':###\n", searchQueryStr.c_str());

	int cntRes = 0;
	for (int i = 0; i < n && res[i].first > searchEngineObject::alpha; ++i) ++cntRes;

	if (cntRes <= 0) wprintf(L"\nYour search - '%s' - did not match any documents.\n", searchQueryStr.c_str());
	else {
		for (int i = 0; i < min(cntRes,15); ++i) {
			wcout << i + 1 << ", " << res[i].second->filePath.c_str() << " (" << res[i].first << ")\n";
		}
		if (cntRes > 15) wcout << "...\n";
		wcout << "\n" << cntRes << " results found in " << (double)((clock() - startTime) / CLOCKS_PER_SEC) << " seconds !!!\n";
	}

	wcout << "\n";

	delete[]res;
	freeMemoryTOKENSLIST(tokenList);
}