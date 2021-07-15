#include "stdafx.h"
#include "filePath.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

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
	data = (char *)malloc(sizeof(char) * (len + 1));
	const wchar_t dic[] = L"áàảãạÁÀẢÃẠăắằẳẵặĂẮẰẲẴẶâấầẩẫậÂẤẦẨẪẬđĐéèẻẽẹÉÈẺẼẸêếềểễệÊẾỀỂỄỆíìỉĩịÍÌỈĨỊóòỏõọÓÒỎÕỌôốồổỗộÔỐỒỔỖỘơớờởỡợƠỚỜỞỠỢúùủũụÚÙỦŨỤưứừửữựƯỨỪỬỮỰýỳỷỹỵÝỲỸỶỴ";
	const char replace[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddeeeeeeeeeeeeeeeeeeeeeeiiiiiiiiiioooooooooooooooooooooooooooooooooouuuuuuuuuuuuuuuuuuuuuuyyyyyyyyyy";
	for (int i = 0; i < len; i++)
	{
		const wchar_t *p = wcschr(dic, wdata[i]);
		if (p != NULL)
			data[i] = replace[p - dic];
		else
			data[i] = (char)towlower(wdata[i]);
	}
	data[len] = '\0';
}

string toLowerCase(string S) {
	std::for_each(S.begin(), S.end(), [](char & c) {
		c = ::tolower(c);
	});
	return S;
}