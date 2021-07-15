#pragma once

#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

string replaceString(string S, string S2, string S3);
string normalizePath(string filePath);

void standardize(wchar_t *wdata, char *&data);
string joinPath(string S, string S2);
string toLowerCase(string S);
//string p1 = "   C:\\Users \\achung0147   /Documents\\Visual Studio 2017\\Projects\\DoanCuoiki - Copy\\DoanCuoiki\\one file train\\";
//string p2 = "C:\\Users \\achung0147\\   Documents\\Visual Studio 2017   \\/  Projects\\DoanCuoiki - Copy\\DoanCuoiki\\one file train\\  "  ;
