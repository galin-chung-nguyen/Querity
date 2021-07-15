#pragma once
#include <stdio.h>
#include <wchar.h>
#include <string>
#include <iostream>
#include "createMetadata.h"
#include <time.h>
#include <io.h>

using namespace std;

// FilePath là đường dẫn tuyệt đối
bool addFile(char *filePath, char *indexFileName, char *metadataFileName, char *stopwordsFileName, string &data);

bool removeFile(char *filePath, char *indexFile);