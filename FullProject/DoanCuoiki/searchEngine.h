#pragma once
#include "stdint.h"
#include <unordered_map>
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

typedef unordered_map<uint32_t, double> hashMap;

struct TextFile{
	string filePath, fileTitle;
	hashMap keyWordMap;
	TextFile* nextFile;
	bool used;

	TextFile(string path = "", string title = "", TextFile* next = NULL) {
		filePath = path;
		fileTitle = title;
		nextFile = next;
		used = true;
	}
};

typedef unordered_map<uint32_t, TextFile*> filePathMap;

struct DataContainer{
	TextFile *textData;
	filePathMap fileMap;

	TextFile* newFile(const char* filePath);
	TextFile* fileExist(const char* filePath);

	void freeMem() {
		auto ptr = textData;
		while (ptr) {
			auto ptr2 = ptr->nextFile;
			delete ptr;
			ptr = ptr2;
		}
		textData = NULL;

		fileMap.clear();
	}

	DataContainer() {
		textData = NULL;
	}
};
struct searchEngineObject {
	static constexpr double alpha = 0.015;
	static uint32_t SuperFastHash(const char * data, int len); // hash function
	static pair<double, TextFile*>* assessRelevance(TextFile* textData, TOKENSLIST tokens, int &cntRes);
	static bool isStopWord(wstring token, TOKENSLIST &stopWords);
	static void search(TextFile* textData, wchar_t rawSearchQuery[], TOKENSLIST &stopWords);
};

// http://www.azillionmonkeys.com/qed/hash.html
uint32_t searchEngineObject::SuperFastHash(const char * data, int len) {
	uint32_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash += get16bits(data);
		tmp = (get16bits(data + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		data += 2 * sizeof(uint16_t);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3: hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += (signed char)*data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

// Data Conatainer definitions
TextFile*  DataContainer::fileExist(const char* filePath) { // check if this file has ever bene added into the data container
	string pathStr = normalizePath((string)filePath);
	auto hashCode = searchEngineObject::SuperFastHash(toLowerCase(pathStr).c_str(), strlen(filePath));
	if (DataContainer::fileMap.find(hashCode) != DataContainer::fileMap.end()) {
		return DataContainer::fileMap[hashCode];
	}
	return NULL;
}

TextFile* DataContainer::newFile(const char* filePath) {
	string pathStr = normalizePath((string)filePath);
	auto hashCode = searchEngineObject::SuperFastHash(toLowerCase(pathStr).c_str(), strlen(filePath));
	if (DataContainer::fileMap.find(hashCode) != DataContainer::fileMap.end())
		return DataContainer::fileMap[hashCode];
	auto res = new TextFile(pathStr,"Just some random title", DataContainer::textData);
	DataContainer::fileMap[hashCode] = res;
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
	
	// generate hashCode for each token

	int nTokens = tokens.nTokens;
	uint32_t *hashCode = new uint32_t[nTokens];

	//wcout << "List keywords : \n";
	auto tokenPtr = tokens.pHead;
	for (int i = 0; i < nTokens; ++i) {
		string tokenStr = "";
		for (int i = 0; i < wcslen(tokenPtr->wdata); ++i) tokenStr += (char)tolower((char)((int)tokenPtr->wdata[i]));

		//wcout << "'" << tokenStr.c_str() << "' ";
		hashCode[i] = searchEngineObject::SuperFastHash(tokenStr.c_str(), tokenStr.size());
		tokenPtr = tokenPtr->pNext;
	}
	//wcout << "\n";

	// assess relevance of each text file

	pair<double, TextFile*> *res = new pair<double, TextFile*>[cntFiles];

	textFilePtr = textData;
	for (int i = 0; i < cntFiles; ++i) {
		while(!textFilePtr->used) textFilePtr = textFilePtr->nextFile;
		res[i] = make_pair(0, textFilePtr);
		for (int j = 0; j < nTokens; ++j) res[i].first += textFilePtr->keyWordMap[hashCode[j]];
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

	delete[]hashCode;
	
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

			addHeadTOKENSLIST((wchar_t *)tokenStr.c_str(), "", tokenList);
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