#pragma once

#include "../util/defines.h"


class INuiSensor;

class Kinect
{
	// ��� ����
private:
	INuiSensor *sensor;
    HANDLE nextDepthFrameEvent;
	HANDLE depthStreamHandle;
	HANDLE nextColorFrameEvent;
	HANDLE colorStreamHandle;
	
	long depthWidth;
	long depthHeight;
	long colorWidth;
	long colorHeight;
	
	ushort *depthBuffer;		///< ���� ����.. free
	byte *colorBuffer;			///< �÷� ����.. free
	byte *mappedColorBuffer;	///< .. free
	long *colorCoordinates;		///< .. free


	// ��� �Լ�
public:
	Kinect(INuiSensor *sensor);
	~Kinect();

	/// Ű��Ʈ ���� ����
	void releaseSensor();

	/// ���� ���� ����
	int refreshDepthBuffer();

	/// �÷� ���� ����
	int refreshColorBuffer();

	/// ����
	int mapColorToDepth();


	// ����
public:
	GETTER_SETTER(long, DepthWidth, depthWidth)
	GETTER_SETTER(long, DepthHeight, depthHeight)
	GETTER_SETTER(long, ColorWidth, colorWidth)
	GETTER_SETTER(long, ColorHeight, colorHeight)
	GETTER(ushort*, DepthBuffer, depthBuffer)
	GETTER(byte*, ColorBuffer, colorBuffer)
	GETTER(byte*, MappedColorBuffer, mappedColorBuffer)
};

