
#include "stdafx.h"
#include "BmpExporter.h"

#include <stdio.h>


/// 저장
void BmpExporter::export(const string &filePath, int width, int height, byte *buffer, bool conversion32to24)
{
	BITMAPFILEHEADER    bf;
	BITMAPINFOHEADER    bi;
	byte *data = buffer;
	byte *conversionBuffer = null;
 
	// 헤더 채우기
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	
	ZeroMemory(&bf, sizeof(BITMAPFILEHEADER));
	bf.bfType = *(WORD*)"BM";
	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


	// 32bit -> 24bit
	if (conversion32to24)
	{
		int size = width * height;
		conversionBuffer = new byte[size * 3];

		for (int i=0; i<size; ++i)
		{
			for (int j=0; j<3; ++j)
			{
				conversionBuffer[i*3 + j] = buffer[i*4 + j];
			}
		}

		data = conversionBuffer;
	}

	
	// 상하 반전
	byte *flippedBuffer = new byte[width * height * 3];
	for (int y=0; y<height; ++y)
	{
		for (int x=0; x<width; ++x)
		{
			byte *src = &data[((height - y - 1)*width + x) * 3];
			byte *dst = &flippedBuffer[(y*width + x) * 3];
			for (int pixelIndex=0; pixelIndex<3; ++pixelIndex)
			{
				dst[pixelIndex] = src[pixelIndex];
			}
		}
	}


	// 파일에 쓰기
	FILE *fp = fopen(filePath.c_str(), "wb");
	fwrite(&bf, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&bi, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);
	fwrite(flippedBuffer, sizeof(unsigned char), bi.biSizeImage, fp);
	fclose(fp);


	DELETE(flippedBuffer);
	DELETE(conversionBuffer);
}

