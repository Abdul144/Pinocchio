#pragma once

#include <vector>

#include "../marker/MarkerRecognizer.h"
#include "../util/defines.h"
#include "../util/Matrix.h"
#include "../util/Vector3.h"


using namespace std;

class CloudElement;
class INuiSensor;
class Vector3;

class Kinect
{
public:
	struct MarkerInfo
	{
		Vector3 v0;
		Vector3 v1;
		Vector3 v2;
		Vector3 v3;
	};

	// 멤버 변수
private:
	INuiSensor *sensor;
    HANDLE nextDepthFrameEvent;
	HANDLE depthStreamHandle;
	HANDLE nextColorFrameEvent;
	HANDLE colorStreamHandle;
	HANDLE nextSkeletonFrameEvent;
	DWORD skeletonTrakingFlags;
	
	long depthWidth;
	long depthHeight;
	long colorWidth;
	long colorHeight;
	Matrix transform;	///< 변환 행렬
	
	ushort *depthBuffer;		///< 깊이 버퍼.. free
	byte *colorBuffer;			///< 컬러 버퍼.. free
	byte *mappedColorBuffer;	///< .. free
	long *colorCoordinates;		///< .. free
	Vector3 *pointCloud;		///< 포인트 클라우드.. free
	CloudElement *background;	///< 배경.. free

	vector<Vector3> skeleton;
	vector<Quaternion> skeletonRotationInfo;

	vector<int> kinectBoneIndexMap;	///< [본 인덱스] => 키넥트 본 인덱스

public:
    static const float xyScale;
	static int magicX, magicY;


	// 멤버 함수
private:
	/// 스켈레톤 회전 정보 갱신
	void refreshSkeletonRotationInfo();

	/// 스켈레톤 회전각 계산
	void refreshSingleSkeletonRotation(int parentBoneIndex, int boneIndex, const Matrix &localizeTransform, Matrix &resultLocalizeTransform);

public:
	Kinect(INuiSensor *sensor, bool useSkeleton);
	~Kinect();

	/// 키넥트 센서 해제
	void releaseSensor();

	/// 깊이 버퍼 갱신
	int refreshDepthBuffer();

	/// 컬러 버퍼 갱신
	int refreshColorBuffer();

	/// 스켈레톤 갱신
	int refreshSkeleton();

	/// 매핑
	int mapColorToDepth();

	/// 스켈레톤 그리기
	void drawSkeleton();

	/// 스켈레톤 정보 저장
	void saveSkeletonInfo();

	/// 변환행렬 구성
	bool setTransformFromMarkerInfo(const MarkerInfo &markerInfo);

	/// 포인트 클라우드에 변환행렬 적용,
	void transformPointCloud(CloudElement *result, bool bgRemovalOption = true);

	/// 스켈레톤에 변환행렬 적용
	void transformSkeleton();

	// 여러번 반복하여 정확하게 마커 인식
	bool recognizeMakerAccurately(int repeatCount, int limitDeadCount);
	
	bool refreshDepthAndColorBuffer();
	bool recognizeMarker(MarkerRecognizer::sMarkerInfo &marker);
	bool isValidMarker(const MarkerRecognizer::sMarkerInfo &marker);
	bool convertMarkerInfo(const MarkerRecognizer::sMarkerInfo &from, MarkerInfo &to);

	void setBackground(CloudElement *bg);

	inline int getKinectBoneIndex(int index)
	{
		return kinectBoneIndexMap[index];
	}


	// 접근
public:
	GETTER_SETTER(long, DepthWidth, depthWidth)
	GETTER_SETTER(long, DepthHeight, depthHeight)
	GETTER_SETTER(long, ColorWidth, colorWidth)
	GETTER_SETTER(long, ColorHeight, colorHeight)
	GETTER(ushort*, DepthBuffer, depthBuffer)
	GETTER(byte*, ColorBuffer, colorBuffer)
	GETTER(byte*, MappedColorBuffer, mappedColorBuffer)
	GETTER(Vector3*, PointCloud, pointCloud)
	GETTER_CONST_REF(Matrix, Transform, transform)
	GETTER_CONST_REF(vector<Quaternion>, SkeletonRotationInfo, skeletonRotationInfo)
};

