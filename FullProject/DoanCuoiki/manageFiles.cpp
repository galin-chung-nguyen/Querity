#include "stdafx.h"

#include <iostream>
#include <sstream>
#include "manageFiles.h"

using namespace std;

// FilePath là đường dẫn tuyệt đối
bool addFile(char *filePath, char *indexFileName, char *metadataFileName, char *stopwordsFileName, string &data)
{
    // Kiểm tra xem file đó có tồn tại không
    FILE *check = fopen(filePath, "r");
    if (!check)
    {
		wcout << "\nError !!! File '" << filePath << "' not found !!!\n";
        return false;
    }
    fclose(check);

	clock_t startTime = clock();

    // Nếu file có tồn tại, thêm địa chỉ file vào index.txt và thêm data vào metadata
    FILE *index = fopen(indexFileName, "a");
    if (!index)
        return false;
    fprintf(index, "add|%s\n", filePath);
    fclose(index);

    // Đọc stopword
    TOKENSLIST stopwords;
    int nStopwords = getStopwords(stopwordsFileName, stopwords);

    // Đọc nội dung file văn bản
    wchar_t *text = readTextFile(filePath);
    if (!text)
        return false;

    // Tách ra các tokens (đồng thời xóa dấu và lọc stopword)
    TOKENSLIST allTokens;
    getTokens(text, allTokens, stopwords);

    // Lấy các token quan trọng
    TOKENSLIST importantTokens;
    getImportantTokens(allTokens, importantTokens);

    // Sắp xếp importantTokens theo chiều giảm dần tần suất xuất hiện của tokens
    mergeSortTOKENSLIST(importantTokens);

    // Ghi kết quả vào tệp tin siêu dữ liệu
    FILE *metadata = fopen(metadataFileName, "a");
    if (metadata == NULL)
        return false;
    fprintf(metadata, "%s\n", filePath);
	for (TOKEN *p = importantTokens.pHead; p; p = p->pNext) {
		fprintf(metadata, "%s[%.5f] ", p->data, p->occurFrequency);
		//  add this to the returned string 
		std::stringstream sstream;
		sstream << p->occurFrequency;
		data += (string)(p->data) + "[" + sstream.str() + "] ";
	}
    fprintf(metadata, "\n");

    fclose(metadata);
    freeMemoryTOKENSLIST(stopwords);
    freeMemoryTOKENSLIST(allTokens);
    freeMemoryTOKENSLIST(importantTokens);

	wcout << "\nFile '" << filePath << "' added successfully !!! (" << (double)((clock() - startTime) / CLOCKS_PER_SEC) <<"s) \n";

    return true;
}

bool removeFile(char *filePath, char *indexFile)
{
    // Kiểm tra xem file có tồn tại không
    FILE *check = fopen(filePath, "r");
    if (!check)
    {
		wcout << "\nError !!! File '" << filePath << "' not found !!!\n";
		return false;
    }
    fclose(check);

    /*wchar_t *wtext = readTextFile(indexFile);
    if (!wtext)
        return false;
    int len = wcslen(wtext);
    char *text = (char *)malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len; i++)
        text[i] = (char(wtext[i]));
    text[len] = '\0';
    free(wtext);
    if (strstr(text, filePath) == NULL)
    {
        free(text);
        printf("File path %s is not exist in index file!\n", filePath);
        return false;
    }
    free(text);*/

	clock_t startTime = clock();

    // Thêm lệnh xóa vào index file
    FILE *index = fopen(indexFile, "a");
    if (!index)
        return false;
    fprintf(index, "delete|%s\n", filePath);
    fclose(index);

	wcout << "\nFile '" << filePath << "' removed successfully !!! (" << (double)((clock() - startTime) / CLOCKS_PER_SEC) << "s) \n";

    return true;
}

/*int main(int argc, char *argv[])
{
    // Command: addFile.exe <file path> <index file> <metadata file>
    if (argc != 5)
        printf("Command Error!\nUsed: addFile.exe <file path> <index file> <metadata file> <stopword file>\n");
    else
        addFile(argv[1], argv[2], argv[3], argv[4]);
}*/