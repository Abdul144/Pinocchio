
#include "stdafx.h"
#include "Kinect.h"

#include <math.h>
#include <NuiApi.h>

#include "../util/Vector3.h"

const float Kinect::xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);
int Kinect::magicX = -47;
int Kinect::magicY = -47;


Kinect::Kinect(INuiSensor *sensor) : sensor(sensor)
{
	HRESULT hr;

	// Ű��Ʈ �ʱ�ȭ
	hr = sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}

	// depth event ����
	nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// depth image stream�� ����
	hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}

	// color event ����
	nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// color image stream�� ����
	hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}
	
	// ������ ���
	DWORD width, height;

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);
	depthWidth = (long)width;
	depthHeight = (long)height;

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);
	colorWidth = (long)width;
	colorHeight = (long)height;


	// ���� �Ҵ�
	depthBuffer = new ushort[depthWidth * depthHeight];
	memset(depthBuffer, 0, sizeof(ushort) * depthWidth * depthHeight);
	colorBuffer = new byte[colorWidth * colorHeight * 4];
	memset(colorBuffer, 0, sizeof(byte) * colorWidth * colorHeight * 4);
	mappedColorBuffer = new byte[colorWidth * colorHeight * 4];
	memset(mappedColorBuffer, 0, sizeof(byte) * colorWidth * colorHeight * 4);
	colorCoordinates = new long[depthWidth * depthHeight * 2];
	memset(colorCoordinates, 0, sizeof(long) * depthWidth * depthHeight * 2);
	pointCloud = new Vector3[depthWidth * depthHeight];
}


Kinect::~Kinect()
{
	releaseSensor();

	DELETE_ARRAY(depthBuffer)
	DELETE_ARRAY(colorBuffer)
	DELETE_ARRAY(mappedColorBuffer)
	DELETE_ARRAY(colorCoordinates)
	DELETE_ARRAY(pointCloud)
}

/// Ű��Ʈ ���� ����
void Kinect::releaseSensor()
{
	if (sensor)
	{
		sensor->NuiShutdown();
		sensor->Release();
	}
}

/// ���� ���� ����
int Kinect::refreshDepthBuffer()
{
	NUI_IMAGE_FRAME imageFrame;
	HRESULT hr;

	if (WaitForSingleObject(nextDepthFrameEvent, 0) != WAIT_OBJECT_0)
		return -1;

	hr = sensor->NuiImageStreamGetNextFrame(depthStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
		return hr;

	NUI_LOCKED_RECT lockedRect;
	hr = imageFrame.pFrameTexture->LockRect(0, &lockedRect, null, 0);
	if (FAILED(hr))
		return hr;

	memcpy(depthBuffer, lockedRect.pBits, lockedRect.size);

	hr = imageFrame.pFrameTexture->UnlockRect(0);
	if (FAILED(hr))
		return hr;

	hr = sensor->NuiImageStreamReleaseFrame(depthStreamHandle, &imageFrame);
	if (FAILED(hr))
		return hr;

	return 0;
}

/// �÷� ���� ����
int Kinect::refreshColorBuffer()
{
	NUI_IMAGE_FRAME imageFrame;
	HRESULT hr;
	
	if (WaitForSingleObject(nextColorFrameEvent, 0) != WAIT_OBJECT_0)
		return -1;

	hr = sensor->NuiImageStreamGetNextFrame(colorStreamHandle, 0, &imageFrame);
	if (FAILED(hr))
		return hr;

	NUI_LOCKED_RECT lockedRect;
	hr = imageFrame.pFrameTexture->LockRect(0, &lockedRect, null, 0);
	if (FAILED(hr))
		return hr;

	memcpy(colorBuffer, lockedRect.pBits, lockedRect.size);

	hr = imageFrame.pFrameTexture->UnlockRect(0);
	if (FAILED(hr))
		return hr;

	hr = sensor->NuiImageStreamReleaseFrame(colorStreamHandle, &imageFrame);
	if (FAILED(hr))
		return hr;

	return 0;
}

/// ����
int Kinect::mapColorToDepth()
{
	HRESULT hr;
    static const float xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);

	sensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
		NUI_IMAGE_RESOLUTION_640x480,
		NUI_IMAGE_RESOLUTION_640x480,
		depthWidth * depthHeight,
		depthBuffer,
		depthWidth * depthHeight * 2,
		colorCoordinates
		);
	
    long colorToDepthDivisor = colorWidth/depthWidth;

	for (long y=0; y<colorHeight; ++y)
	{
		uint *dest = (uint*)mappedColorBuffer + y * colorWidth;
		for (long x=0; x<colorWidth; ++x)
		{
			int depthIndex = x/colorToDepthDivisor + y/colorToDepthDivisor * depthWidth;

			long colorInDepthX = colorCoordinates[depthIndex * 2];
			long colorInDepthY = colorCoordinates[depthIndex * 2  + 1];

			if (colorInDepthX >= 0 && colorInDepthX < colorWidth && colorInDepthY >= 0 && colorInDepthY < colorHeight)
			{
				long colorIndex = colorInDepthX + colorInDepthY * colorWidth;

				uint *src = (uint*)colorBuffer + colorIndex;
				*dest = *src;
			}
			else
			{
				*dest = 0;
			}

			// ����Ʈ Ŭ���� ����
			ushort depth = depthBuffer[depthIndex] >> 3;
			float realDepth = (float)depth / 1000.f;
			if (depth >= 300 && depth <= 4000)
			{
				pointCloud[depthIndex].set((x - 320 + magicX) * realDepth * xyScale, (480 - y - 240 + magicY) * realDepth * xyScale, -realDepth);
			}else
			{
				pointCloud[depthIndex].set((x - 320) * realDepth * xyScale, (480 - y - 240) * realDepth * xyScale, 10000);
			}

			dest++;
		}
	}

	return 0;
}