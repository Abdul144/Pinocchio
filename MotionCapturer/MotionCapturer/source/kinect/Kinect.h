#pragma once

#include "../util/defines.h"


class INuiSensor;

class Kinect
{
	// 멤버 변수
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
	
	ushort *depthBuffer;		///< 깊이 버퍼.. free
	byte *colorBuffer;			///< 컬러 버퍼.. free
	byte *mappedColorBuffer;	///< .. free
	long *colorCoordinates;		///< .. free


	// 멤버 함수
public:
	Kinect(INuiSensor *sensor);
	~Kinect();

	/// 키넥트 센서 해제
	void releaseSensor();

	/// 깊이 버퍼 갱신
	int refreshDepthBuffer();

	/// 컬러 버퍼 갱신
	int refreshColorBuffer();

	/// 매핑
	int mapColorToDepth();


	// 접근
public:
	GETTER_SETTER(long, DepthWidth, depthWidth)
	GETTER_SETTER(long, DepthHeight, depthHeight)
	GETTER_SETTER(long, ColorWidth, colorWidth)
	GETTER_SETTER(long, ColorHeight, colorHeight)
	GETTER(ushort*, DepthBuffer, depthBuffer)
	GETTER(byte*, ColorBuffer, colorBuffer)
	GETTER(byte*, MappedColorBuffer, mappedColorBuffer)
};

