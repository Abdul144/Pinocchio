
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


// Ű��Ʈ ���� Ȯ��.. �߰� ����� ���� ����
int KinectManager::connectKinects(int count)
{
	HRESULT hr;
	INuiSensor *sensor = null;
	int addedCount = 0;
	
	// ����� ���� ���
	int sensorCount = 0;
	hr = NuiGetSensorCount(&sensorCount);
	if (FAILED(hr))
		return hr;

	// ����� Ű��Ʈ ��ȸ
	for (int i=getKinectCount(); i<sensorCount; ++i)
	{
		if (addedCount == count)
			break;

		// ���� ����
		hr = NuiCreateSensorByIndex(i, &sensor);
		if (FAILED(hr))
			continue;

		// ���¸� ���, ����Ǿ������� �ʱ�ȭ
		hr = sensor->NuiStatus();
		if (FAILED(hr))
		{
			sensor->Release();
			continue;
		}

		// ���� �߰�
		Kinect *newKinect = new Kinect(sensor);
		kinects.push_back(newKinect);
		addedCount++;
	}

	return addedCount;
}

// Ű��Ʈ ���� ��� ����
void KinectManager::deconnectKinects()
{
	// Ű��Ʈ ����
	for (int i=0; i<kinects.size(); ++i)
	{
		DELETE(kinects[i]);
	}
	kinects.clear();
}