// RGB2Gray.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


//This is used for transform GRB image stored in Bitmap format to GrayScale

#include <iostream>
#include <fstream>
#include "RGB2Gray.h"

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "Please input the origin and post-processed image file name" << endl;
		return -1;
	}
	string iFile(argv[argc - 2]);
	string wFile(argv[argc - 1]);


	//Read origin bitmap data
	int w, h;
	unsigned char* iData;

	iData = ReadBitmapRGB(iFile, w, h);
	
	//Post-data 
	unsigned char* pData = new unsigned char[w*h];
	unsigned char temp;
	
	for (int i = 0; i < w*h ; i++)
	{
		temp = unsigned char(
			(float)iData[3 * i] * 0.114 +
			(float)iData[3 * i + 1] * 0.587 +
			(float)iData[3 * i + 2] * 0.299
			);
		if (temp > 255)
		{
			temp = 255;
		}

		pData[i] = temp;
	}

	//Write Gray bitmap 

	if (!WriteBitmapRGB(wFile,w,h,pData,8))
	{
		cout << "Failed to write post-processed image file" << endl;
		return -1;
	}

	return 0;
}


bool WriteBitmapRGB(string sFilename,
	int w, int h, unsigned char* Data, int BitCount)
{
	// The bitsize
	int bitsize = (w * BitCount + 31) / 32 * 4 * h;

	ofstream sFile(sFilename, ios::out | ios::binary);
	if (!sFile)
	{
		cout << "Error: Can not write the file "
			<< "\"" << sFilename << "\"." << endl;

		return false;
	}

	// Write the BMP file header
	BITMAPFILEHEADER* pHeader = new BITMAPFILEHEADER;
	pHeader->bfType = 'MB';
	pHeader->bfReserved1 = 0;
	pHeader->bfReserved2 = 0;
	switch (BitCount)
	{
	case 1:
		break;

	case 4:
		break;

	case 8:
		pHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
		break;

	case 24:
		pHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		break;

	}
	pHeader->bfSize = pHeader->bfOffBits + bitsize;

	char* TmpHeader = (char*)(pHeader);
	sFile.write(TmpHeader, sizeof(BITMAPFILEHEADER));
	delete pHeader;		pHeader = NULL;

	// Write the BMP info header
	BITMAPINFOHEADER* pInfo = new BITMAPINFOHEADER;
	pInfo->biSize = sizeof(BITMAPINFOHEADER);
	pInfo->biWidth = w;
	pInfo->biHeight = h;
	pInfo->biPlanes = 1;
	pInfo->biBitCount = BitCount;
	pInfo->biCompression = BI_RGB;
	pInfo->biSizeImage = 0;
	pInfo->biXPelsPerMeter = 0;
	pInfo->biYPelsPerMeter = 0;
	pInfo->biClrImportant = 0;
	pInfo->biClrUsed = 0;

	char* TmpInfo = (char*)(pInfo);
	sFile.write(TmpInfo, sizeof(BITMAPINFOHEADER));
	delete pInfo;		pInfo = NULL;

	// Write the BMP data
	unsigned char* TmpData = new unsigned char[bitsize];

	RGBQUAD *rgbQuad = new RGBQUAD[256];
	for (int i = 0; i < 256; i++)
	{
		rgbQuad[i].rgbBlue = i;
		rgbQuad[i].rgbGreen = i;
		rgbQuad[i].rgbRed = i;
		rgbQuad[i].rgbReserved = 0;
	}

	int iWidth = 0;
	switch (BitCount)
	{
	case 1:
		break;

	case 4:
		break;

	case 8:			// For the 8-bit(256-Colors) Bitmap
		iWidth = bitsize / h;
		for (int i = 0; i < bitsize; i++)
		{
			int iy = i / iWidth;
			int ix = i - iy*iWidth;

			if (ix < w)
			{
				TmpData[i] = Data[iy*w + ix];
			}
			else
			{
				TmpData[i] = 0;
			}
		}
		sFile.write((char*)rgbQuad, sizeof(RGBQUAD) * 256);
		sFile.write((char*)TmpData, bitsize);
		break;

	case 24:		// For the 24-bit Bitmap
		iWidth = bitsize / h;
		for (int i = 0; i < bitsize; i++)
		{
			int iy = i / iWidth;
			int ix = i - iy*iWidth;

			if (ix < w * 3)
			{
				TmpData[i] = Data[iy*w * 3 + ix];
			}
			else
			{
				TmpData[i] = 0;
			}
		}
		sFile.write((char*)TmpData, bitsize);
		break;

	default:
		cout << "Error: "
			<< "This program does not support this format of the bitmap"
			<< endl;

		return false;
	}
	sFile.close();

	return true;
}

unsigned char* ReadBitmapRGB(string sFilename,
	int& w, int& h)
{
	ifstream sFile(sFilename, ios::in | ios::binary);

	if (!sFile)
	{
		cout << "Error: Can not open the file "
			<< "\"" << sFilename << "\"." << endl;

		return NULL;
	}

	// Read the BMP file header
	BITMAPFILEHEADER* pHeader;
	char* TmpHeader = new char[sizeof(BITMAPFILEHEADER)];

	sFile.read(TmpHeader, sizeof(BITMAPFILEHEADER));
	pHeader = (BITMAPFILEHEADER*)(TmpHeader);
	int i = pHeader->bfOffBits;
	int j = sizeof(BITMAPFILEHEADER);
	int k = sizeof(BITMAPINFOHEADER);

	if (pHeader->bfType != 0x4D42)
	{
		cout << "Error: The file "
			<< "\"" << sFilename << "\" is not a Bitmap." << endl;

		delete[] TmpHeader;	TmpHeader = NULL;
		return NULL;
	}

	delete[] TmpHeader;	TmpHeader = NULL;

	// Read the BMP info header
	BITMAPINFOHEADER* pInfo;
	char* TmpInfo = new char[sizeof(BITMAPINFOHEADER)];

	sFile.read(TmpInfo, sizeof(BITMAPINFOHEADER));
	pInfo = (BITMAPINFOHEADER*)(TmpInfo);

	// The width and height of the BMP
	w = (int)(pInfo->biWidth);
	h = (int)(pInfo->biHeight);
	DWORD	biCompression = pInfo->biCompression;
	int		biBitCount = (int)(pInfo->biBitCount);
	int		biSizeImage = (int)(pInfo->biSizeImage);

	delete[] TmpInfo;		TmpInfo = NULL;

	// Read the Data of the BMP
	RGBQUAD *TmpColor = new RGBQUAD[256];
	switch (biBitCount)
	{
	case 1:
		break;

	case 4:
		break;

	case 8:
		sFile.read((char *)TmpColor, sizeof(RGBQUAD) * 256);

		delete[] TmpColor;        TmpColor = NULL;
		break;

	case 24:
		break;

	default:
		break;
	}

	biSizeImage = (w * biBitCount + 31) / 32 * 4 * h;

	char* TmpData = new char[biSizeImage];

	if (!TmpData)
	{
		cout << "Error: "
			<< "There are not enough memory for the BMP data."
			<< endl;

		delete[] TmpData;	TmpData = NULL;
		return NULL;
	}

	sFile.read(TmpData, biSizeImage);
	sFile.close();

	// Re-arrange the data
	unsigned char* Data = NULL;
	switch (biBitCount)
	{
	case 1:
		break;

	case 4:
		break;

	case 8:
		Data = new unsigned char[w*h];
		break;

	case 24:
		Data = new unsigned char[w*h * 3];
		break;

	default:
		break;
	}
	if (!Data)
	{
		cout << "Error: "
			<< "There are not enough memory for the BMP data."
			<< endl;

		delete[] TmpData;	TmpData = NULL;
		delete[] Data;		Data = NULL;
		return NULL;
	}

	int iWidth = 0;
	switch (biBitCount)
	{
	case 1:
		break;

	case 4:
		break;

	case 8:			// For the 8-bit(256-Colors) Bitmap
		iWidth = biSizeImage / h;
		for (int i = 0; i < biSizeImage; i++)
		{
			int iy = i / iWidth;
			int ix = i - iy*iWidth;

			if (ix < w)
			{
				Data[iy*w + ix] = TmpData[i];
			}
		}
		break;

	case 24:		// For the 24-bit Bitmap
		iWidth = biSizeImage / h;
		for (int i = 0; i < biSizeImage; i++)
		{
			int iy = i / iWidth;
			int ix = i - iy*iWidth;

			if (ix < w * 3)
			{
				Data[iy*w * 3 + ix] = TmpData[i];
			}
		}
		break;

	default:
		cout << "Error: "
			<< "This program does not support this format of the bitmap"
			<< endl;

		delete[] TmpData;	TmpData = NULL;
		delete[] Data;		Data = NULL;
		return NULL;
	}

	delete[] TmpData;		TmpData = NULL;

	return Data;
}