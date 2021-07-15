#include "stdafx.h"
#include "filePath.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "TOKENSLIST.h"

using namespace std;

string replaceString(string S, string S2, string S3) {
	while (true) {
		std::size_t found = S.find(S2);
		if (found == std::string::npos) break;
		S.replace(found, S2.length(), S3);
	}
	return S;
}
string normalizePath(string filePath) {
	string res = filePath;
	res = replaceString(res, "\\", "/");
	res = replaceString(res, "//", "/");
	res = replaceString(res, "/ ", "/");
	res = replaceString(res, " /", "/");
	while (res.size() && (res.back() == ' ' || res.back() == '/')) res.pop_back();
	std::reverse(res.begin(), res.end());
	while (res.size() && res.back() == ' ') res.pop_back();
	std::reverse(res.begin(), res.end());
	return res;
}

string joinPath(string S, string S2) {
	S = normalizePath(S);
	S2 = normalizePath(S2);
	
	if (S2.size() && S2[0] == '.') {
		reverse(S2.begin(), S2.end());
		while (S2.size() && S2.back() == '.') S2.pop_back();
		reverse(S2.begin(), S2.end());
	}

	if (S2.size() && S2[0] != '/') S += '/';
	return S + S2;
}


// Hàm tiêu chuẩn hóa một chuỗi tiếng việt có dấu thành không dấu và không viết hoa
void standardize(wchar_t *wdata, char *&data)
{
	int len = wcslen(wdata);
	const wchar_t dic[] = L"áàảãạÁÀẢÃẠăắằẳẵặĂẮẰẲẴẶâấầẩẫậÂẤẦẨẪẬđĐéèẻẽẹÉÈẺẼẸêếềểễệÊẾỀỂỄỆíìỉĩịÍÌỈĨỊóòỏõọÓÒỎÕỌôốồổỗộÔỐỒỔỖỘơớờởỡợƠỚỜỞỠỢúùủũụÚÙỦŨỤưứừửữựƯỨỪỬỮỰýỳỷỹỵÝỲỸỶỴ";
	const char replace[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddeeeeeeeeeeeeeeeeeeeeeeiiiiiiiiiioooooooooooooooooooooooooooooooooouuuuuuuuuuuuuuuuuuuuuuyyyyyyyyyy";
	for (int i = 0; i < len; i++)
	{
		const wchar_t *p = wcschr(dic, wdata[i]);
		if (p != NULL)
			wdata[i] = replace[p - dic];
		else {
			wdata[i] = towlower(wdata[i]);
			if (int(wdata[i]) > 126) wdata[i] = ' ';
		}
	}
	string res = "";
	for (int i = 0; i < len; ++i) if (wdata[i] != ' ') res += (char)wdata[i];

	data = (char *)malloc(sizeof(char) * (res.size() + 1));
	strcpy(data, res.c_str());
	data[res.size()] = '\0';
}

string toLowerCase(string S) {
	std::for_each(S.begin(), S.end(), [](char & c) {
		c = ::tolower(c);
	});
	return S;
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
	//wcout << "Get token for " << text << "\n";
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