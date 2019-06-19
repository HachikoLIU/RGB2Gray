#pragma once

#include "windows.h"
#include <string>

using namespace std;

RGBQUAD *pColorTable;

bool WriteBitmapRGB(string sFilename, int w, int h, unsigned char* Data, int BitCount);
unsigned char* ReadBitmapRGB(string sFilename, int& w, int& h);