
#include "stdafx.h"
#include "KinectManager.h"

#include <NuiApi.h>

#include "../util/defines.h"
#include "Kinect.h"


void CALLBACK kinectDeviceStatusCallBack( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData )
{
}

KinectManager::KinectManager()
{
	NuiSetDeviceStatusCallback(kinectDeviceStatusCallBack, null);
}

KinectManager::~KinectManager()
{
	deconnectKinects();
}


// 키넥트 연결 확보.. 추가 연결된 개수 리턴
int KinectManager::connectKinects(int count)
{
	HRESULT hr;
	INuiSensor *sensor = null;
	int addedCount = 0;
	
	// 연결된 개수 얻기
	int sensorCount = 0;
	hr = NuiGetSensorCount(&sensorCount);
	if (FAILED(hr))
		return hr;

	// 연결된 키넥트 순회
	for (int i=getKinectCount(); i<sensorCount; ++i)
	{
		if (addedCount == count)
			break;

		// 센서 연결
		hr = NuiCreateSensorByIndex(i, &sensor);
		if (FAILED(hr))
			continue;

		// 상태를 얻고, 연결되어있으면 초기화
		hr = sensor->NuiStatus();
		if (FAILED(hr))
		{
			sensor->Release();
			continue;
		}

		// 센서 추가
		Kinect *newKinect = new Kinect(sensor);
		kinects.push_back(newKinect);
		addedCount++;
	}

	return addedCount;
}

// 키넥트 연결 모두 끊기
void KinectManager::deconnectKinects()
{
	// 키넥트 해제
	for (int i=0; i<kinects.size(); ++i)
	{
		DELETE(kinects[i]);
	}
	kinects.clear();
}