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

	// ��� ����
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
	Matrix transform;	///< ��ȯ ���
	
	ushort *depthBuffer;		///< ���� ����.. free
	byte *colorBuffer;			///< �÷� ����.. free
	byte *mappedColorBuffer;	///< .. free
	long *colorCoordinates;		///< .. free
	Vector3 *pointCloud;		///< ����Ʈ Ŭ����.. free
	CloudElement *background;	///< ���.. free

	vector<Vector3> skeleton;
	vector<Quaternion> skeletonRotationInfo;

	vector<int> kinectBoneIndexMap;	///< [�� �ε���] => Ű��Ʈ �� �ε���

public:
    static const float xyScale;
	static int magicX, magicY;


	// ��� �Լ�
private:
	/// ���̷��� ȸ�� ���� ����
	void refreshSkeletonRotationInfo();

	/// ���̷��� ȸ���� ���
	void refreshSingleSkeletonRotation(int parentBoneIndex, int boneIndex, const Matrix &localizeTransform, Matrix &resultLocalizeTransform);

public:
	Kinect(INuiSensor *sensor, bool useSkeleton);
	~Kinect();

	/// Ű��Ʈ ���� ����
	void releaseSensor();

	/// ���� ���� ����
	int refreshDepthBuffer();

	/// �÷� ���� ����
	int refreshColorBuffer();

	/// ���̷��� ����
	int refreshSkeleton();

	/// ����
	int mapColorToDepth();

	/// ���̷��� �׸���
	void drawSkeleton();

	/// ���̷��� ���� ����
	void saveSkeletonInfo();

	/// ��ȯ��� ����
	bool setTransformFromMarkerInfo(const MarkerInfo &markerInfo);

	/// ����Ʈ Ŭ���忡 ��ȯ��� ����,
	void transformPointCloud(CloudElement *result, bool bgRemovalOption = true);

	/// ���̷��濡 ��ȯ��� ����
	void transformSkeleton();

	// ������ �ݺ��Ͽ� ��Ȯ�ϰ� ��Ŀ �ν�
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


	// ����
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

