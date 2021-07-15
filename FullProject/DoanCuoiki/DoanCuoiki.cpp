// DoanCuoiki.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string.h>
#include <wchar.h>
#include "dirent.h"
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include "TOKENSLIST.h"
#include "filePath.h"
#include "createMetadata.h"
#include "searchEngine.h"
#include "manageFiles.h"
#include <windows.h>
#include "fileapi.h"
#include <tchar.h>

using namespace std;

void setUtf16Mode()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* search engine */

string toString(wstring S) {
	string res = "";
	for (int i = 0; i < S.size(); ++i) res += (char)S[i];
	return res;
}

void extractTokensIntoTextData(TextFile *newFile, char dataLine[]) {
	/* start splitting this line into keywords */
	stringstream lineStream;
	string curLine(dataLine);

	lineStream << curLine;

	string tmp;
	while (lineStream >> tmp) { // get each keyword in the line
		tmp.pop_back();
		string keyword = "", scoreStr = ""; // keyword[score]
		for (int i = 0, pos = 0; i < tmp.size(); ++i) {
			if (pos < 1) {
				if (tmp[i] != '[') keyword += tmp[i];
				else ++pos;
			}
			else scoreStr += tmp[i];
		}

		double score = stod(scoreStr); // score of the keyword
		auto hashCode = searchEngineObject::SuperFastHash(keyword.c_str(), keyword.size()); // hashCode of the keyword

		newFile->keyWordMap[hashCode] = score;
		//cout << keyword << " " << score << " ~> " << hashCode << "\n";
	}
}
void loadMetadata(const char* rootDirectory, DataContainer &metadataContainer) {
	wcout << L"Loading data from metadata file ...\n";

	// load stop words

	clock_t startTime = clock();

	char line[5000];

	FILE *metadata = fopen(((string)rootDirectory + "/metadata.txt").c_str(), "r");
	FILE *indexFile = fopen(((string)rootDirectory + "/index.txt").c_str(), "r");
	if (metadata == NULL)
	{
		wcout << L"Cannot open metadata file !!!\n";
		return ;
	}
	if (indexFile == NULL)
	{
		wcout << L"Cannot open index file !!!\n";
		return;
	}

	char tmp, filePath[1234], dataLine[5000];
	int state;

	/*while (fscanf(indexFile, "%[^|]", &state)) { // read each record in index.txt
		if (strcmp(state, "add") != 0 && strcmp(state, "delete") != 0) break;
		fscanf(indexFile, "%c", &tmp);
		fgets(filePath, 1000, indexFile);
		if (filePath[strlen(filePath) - 1]) filePath[strlen(filePath) - 1] = '\0';

		while ((tmp = fgetc(indexFile)) != EOF) {
			if (tmp == '\n') break;
		}*/
	while(fgets(dataLine, 1000, indexFile)){
		if(dataLine[strlen(dataLine) - 1]) dataLine[strlen(dataLine) - 1] = '\0';

		if (dataLine[0] == 'a') {
			state = 1;
			strcpy(filePath, dataLine + 4);
		}
		else if (dataLine[0] == 'd') {
			state = 0;
			strcpy(filePath, dataLine + 7);
		}
		else break;

		//wcout << (state == 0 ? "delete" : "add") << " ~> " << filePath << endl;
		if (state == 0) { // delete action
			TextFile* removedFile = metadataContainer.fileExist(filePath);
			if (removedFile) removedFile->used = false;
			continue;
		}
		if (!fgets(dataLine, 5000, metadata) || !fgets(dataLine, 5000, metadata)) break;
		if (dataLine[strlen(dataLine) - 1] == '\n') dataLine[strlen(dataLine) - 1] = '\0';

		// add action
		TextFile* addedFile = metadataContainer.fileExist(filePath);
		if (addedFile && addedFile->used) continue;
		addedFile = metadataContainer.newFile(filePath);
		addedFile->used = true;
		extractTokensIntoTextData(addedFile, dataLine);
	}

	fclose(metadata);
	fclose(indexFile);

	wprintf(L"Finished ! Loading metadata time: %.0lf seconds.\n\n", (double)((clock() - startTime) / CLOCKS_PER_SEC));
	return;
}

int main()
{
	setUtf16Mode();
	string rootDirectory = "./"; // this is the root directory which contains index.txt & text files

	TOKENSLIST stopwords; // load stopwords
	getStopwords(((string)rootDirectory + "/stopword_one_word.txt").c_str(), stopwords);

	DataContainer metadataContainer;
	loadMetadata(rootDirectory.c_str(), metadataContainer); // load data from metadata file

	while (true) {
		wprintf(L"___________________________________________\n");
		wprintf(L"\nPlease choose one of the below options : \n\n");
		wprintf(L"0. Generate metadata (type 0)\n");
		wprintf(L"1. Search (type 1)\n");
		wprintf(L"2. Add a document (type 2)\n");
		wprintf(L"3. Remove a document (type 3)\n");
		wprintf(L"4. Exit (type 4)\n");

		int op;
		wscanf(L"%d", &op);

		while (op < 0 || op > 4) {
			wprintf(L"Please choose again !\n");
			while (getwc(stdin) != L'\n'); // clear the input buffer
			wscanf(L"%d", &op);
		}
		while (getwc(stdin) != L'\n'); // clear the input buffer

		if (op == 0) {
			wprintf(L"\n#0. Re-generate metadata\n\n");
			generateMetadata("train", "stopword_one_word.txt", "index.txt", "metadata.txt");
			metadataContainer.freeMem();
			loadMetadata(rootDirectory.c_str(), metadataContainer);
		}else if (op == 1) {
			wprintf(L"\n#1. Search\n\n");
			wprintf(L"What do you want to search ? ");
			wchar_t rawSearchQuery[2000];
			//wscanf(L"%s", rawSearchQuery);
			wcin.getline(rawSearchQuery, 2000);
			searchEngineObject::search(metadataContainer.textData, rawSearchQuery, stopwords);
		}
		else if (op == 2) {
			wprintf(L"\n#2. Add a document\n\n");
			wprintf(L"Type the absolute path to the document you want to add :");
			wchar_t tmp[2000];
			wcin.getline(tmp, 2000);

			auto filePath = normalizePath(toString(wstring(tmp)));

			TextFile* addedFile = metadataContainer.fileExist(filePath.c_str());
			if (!addedFile || !addedFile->used) {
				string lineData = "";
				bool check = addFile((char*)filePath.c_str(), "./index.txt", "./metadata.txt", "./stopword_one_word.txt", lineData);
				//wcout << "Data after added new file : " << lineData.c_str() << "\n";
				if (check == false) {/*wcout << "INVALID FILE'S PATH !!!\n";*/ }
				else {
					addedFile = metadataContainer.newFile(filePath.c_str());
					addedFile->used = true;
					extractTokensIntoTextData(addedFile, (char*)lineData.c_str());
					/*wcout << "Adding file '" << filePath.c_str() << "' successfully !!!\n"; */
				}
			} else {
				wcout << "\nPlease remove this file before you add it again !!!\n";
			}
		}
		else if (op == 3) {
			wprintf(L"\n#3. Remove a document\n\n");
			wprintf(L"Type the absolute path to the document you want to remove :");

			wchar_t tmp[2000];
			wcin.getline(tmp, 12345);

			auto filePath = normalizePath(toString(wstring(tmp)));

			TextFile* removedFile = metadataContainer.fileExist(filePath.c_str());
			if (removedFile && removedFile->used) {
				bool check = removeFile((char*)filePath.c_str(), "./index.txt");
				if (check == false) {/*wcout << "INVALID FILE'S PATH !!!\n";*/ }
				else {
					removedFile->used = false;
					/*wcout << "Adding file '" << filePath.c_str() << "' successfully !!!\n"; */
				}
			}else {
				wcout << "\nFile not added yet !!!\n";
			}

		}else {
			// release memory here 
			wprintf(L"\nProgram has finished !!!");
			break;
		}
	}
	metadataContainer.freeMem();
	freeMemoryTOKENSLIST(stopwords);

	return 0;
}