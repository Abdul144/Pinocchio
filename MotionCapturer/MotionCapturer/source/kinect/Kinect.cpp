
#include "stdafx.h"
#include "Kinect.h"

#include <GL/glew.h>
#include <math.h>
#include <NuiApi.h>

#include "../util/Vector3.h"
#include "../Engine.h"
#include "../model/Bone.h"

const float Kinect::xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);
int Kinect::magicX = -47;
int Kinect::magicY = -47;

// TODO test
extern HDC hDC;

Kinect::Kinect(INuiSensor *sensor, bool useSkeleton) :
		sensor(sensor), 
		nextDepthFrameEvent(null),
		depthStreamHandle(null),
		nextColorFrameEvent(null),
		colorStreamHandle(null),
		nextSkeletonFrameEvent(null),
		skeletonTrakingFlags(0),
		background(null)
{
	HRESULT hr;
	DWORD flag;

	if (useSkeleton)
		flag = NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON;
	else
		flag = NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH;


	// 키넥트 초기화
	hr = sensor->NuiInitialize(flag);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}

	// 스켈레톤 설정
	if (useSkeleton)
	{
		nextSkeletonFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

		skeletonTrakingFlags = NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE;
		hr = sensor->NuiSkeletonTrackingEnable(nextSkeletonFrameEvent, skeletonTrakingFlags);

		skeleton.resize(NUI_SKELETON_POSITION_COUNT);
		skeletonRotationInfo.resize(NUI_SKELETON_POSITION_COUNT);
	}

	// depth event 생성
	nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// depth image stream을 생성
	if (useSkeleton)
		hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	else
		hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}

	// color event 생성
	nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// color image stream을 생성
	hr = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(hr))
	{
		releaseSensor();
		return;
	}
	
	// 사이즈 얻기
	DWORD width, height;

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);
	depthWidth = (long)width;
	depthHeight = (long)height;

	NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);
	colorWidth = (long)width;
	colorHeight = (long)height;


	// 버퍼 할당
	depthBuffer = new ushort[depthWidth * depthHeight];
	memset(depthBuffer, 0, sizeof(ushort) * depthWidth * depthHeight);
	colorBuffer = new byte[colorWidth * colorHeight * 4];
	memset(colorBuffer, 0, sizeof(byte) * colorWidth * colorHeight * 4);
	mappedColorBuffer = new byte[colorWidth * colorHeight * 4];
	memset(mappedColorBuffer, 0, sizeof(byte) * colorWidth * colorHeight * 4);
	colorCoordinates = new long[depthWidth * depthHeight * 2];
	memset(colorCoordinates, 0, sizeof(long) * depthWidth * depthHeight * 2);
	pointCloud = new Vector3[depthWidth * depthHeight];
	background = null;

	// 변환행렬 초기화
	transform.setIdentity();

	kinectBoneIndexMap.resize(NUI_SKELETON_POSITION_COUNT);
	kinectBoneIndexMap[Bone::Hip_Center] = NUI_SKELETON_POSITION_HIP_CENTER;
	kinectBoneIndexMap[Bone::Spine] = NUI_SKELETON_POSITION_SPINE;
	kinectBoneIndexMap[Bone::Shoulder_Center] = NUI_SKELETON_POSITION_SHOULDER_CENTER;
	kinectBoneIndexMap[Bone::Head] = NUI_SKELETON_POSITION_HEAD;
	
	kinectBoneIndexMap[Bone::Shoulder_Right] = NUI_SKELETON_POSITION_SHOULDER_LEFT;
	kinectBoneIndexMap[Bone::Elbow_Right] = NUI_SKELETON_POSITION_ELBOW_LEFT;
	kinectBoneIndexMap[Bone::Wrist_Right] = NUI_SKELETON_POSITION_WRIST_LEFT;
	kinectBoneIndexMap[Bone::Hand_Right] = NUI_SKELETON_POSITION_HAND_LEFT;
	
	kinectBoneIndexMap[Bone::Shoulder_Left] = NUI_SKELETON_POSITION_SHOULDER_RIGHT;
	kinectBoneIndexMap[Bone::Elbow_Left] = NUI_SKELETON_POSITION_ELBOW_RIGHT;
	kinectBoneIndexMap[Bone::Wrist_Left] = NUI_SKELETON_POSITION_WRIST_RIGHT;
	kinectBoneIndexMap[Bone::Hand_Left] = NUI_SKELETON_POSITION_HAND_RIGHT;

	kinectBoneIndexMap[Bone::Hip_Right] = NUI_SKELETON_POSITION_HIP_LEFT;
	kinectBoneIndexMap[Bone::Knee_Right] = NUI_SKELETON_POSITION_KNEE_LEFT;
	kinectBoneIndexMap[Bone::Ankle_Right] = NUI_SKELETON_POSITION_ANKLE_LEFT;
	kinectBoneIndexMap[Bone::Foot_Right] = NUI_SKELETON_POSITION_FOOT_LEFT;

	kinectBoneIndexMap[Bone::Hip_Left] = NUI_SKELETON_POSITION_HIP_RIGHT;
	kinectBoneIndexMap[Bone::Knee_Left] = NUI_SKELETON_POSITION_KNEE_RIGHT;
	kinectBoneIndexMap[Bone::Ankle_Left] = NUI_SKELETON_POSITION_ANKLE_RIGHT;
	kinectBoneIndexMap[Bone::Foot_Left] = NUI_SKELETON_POSITION_FOOT_RIGHT;
}


Kinect::~Kinect()
{
	releaseSensor();

	DELETE_ARRAY(depthBuffer)
	DELETE_ARRAY(colorBuffer)
	DELETE_ARRAY(mappedColorBuffer)
	DELETE_ARRAY(colorCoordinates)
	DELETE_ARRAY(pointCloud)
	DELETE_ARRAY(background)
}

/// 키넥트 센서 해제
void Kinect::releaseSensor()
{
	if (sensor)
	{
		skeleton.clear();

		sensor->NuiShutdown();

		if (nextSkeletonFrameEvent && nextSkeletonFrameEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(nextSkeletonFrameEvent);
			nextSkeletonFrameEvent = NULL;
		}
		
		if (nextDepthFrameEvent && nextDepthFrameEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(nextDepthFrameEvent);
			nextDepthFrameEvent = NULL;
		}
		
		if (nextColorFrameEvent && nextColorFrameEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(nextColorFrameEvent);
			nextColorFrameEvent = NULL;
		}

		sensor->Release();
	}
}

/// 깊이 버퍼 갱신
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

/// 컬러 버퍼 갱신
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

/// 스켈레톤 갱신
int Kinect::refreshSkeleton()
{
    static const float xyScale = tanf(deg2rad(58.5f) * 0.5f) / (640.f * 0.5f);

	if (nextSkeletonFrameEvent == null || WaitForSingleObject(nextSkeletonFrameEvent, 0) != WAIT_OBJECT_0)
		return -1;

	NUI_SKELETON_FRAME skeletonFrame = {0};
	HRESULT hr;
	
	hr = sensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
	if (FAILED(hr))
		return hr;
	
    bool foundSkeleton = false;
	int trackedIndex = -1;
    for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

        if ( trackingState == NUI_SKELETON_TRACKED || trackingState == NUI_SKELETON_POSITION_ONLY )
        {
            foundSkeleton = true;
			trackedIndex = i;
        }
    }

    // no skeletons!
    if( !foundSkeleton )
        return 0;

    // smooth out the skeleton data
    hr = sensor->NuiTransformSmooth(&skeletonFrame,NULL);
    if ( FAILED(hr) )
        return -1;
	
	// 스켈레톤 정보 받아 놓기
    for (int i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
    {
		if (skeletonFrame.SkeletonData[trackedIndex].eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_TRACKED)
		{
			Vector4 &pos = skeletonFrame.SkeletonData[trackedIndex].SkeletonPositions[i];
			
			skeleton[i].set(pos.x + magicX * pos.z * xyScale, pos.y + magicY * pos.z * xyScale, -pos.z);
		}else
		{
			skeleton[i].set(0.f, 0.f, 0.f);
		}
	}

	// 각도 정보 갱신
	refreshSkeletonRotationInfo();

	return 0;
}

/// 매핑
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

			// 포인트 클라우드 구성
			int index = depthBuffer[depthIndex] & 7;
			ushort depth = depthBuffer[depthIndex] >> 3;
			float realDepth = (float)depth / 1000.f;
			if (depth >= 300 && depth <= 4000)
			{
				pointCloud[depthIndex].set((x - 320 + magicX) * realDepth * xyScale, (480 - y - 240 + magicY) * realDepth * xyScale, -realDepth);
			}else
			{
				pointCloud[depthIndex].set((x - 320) * realDepth * xyScale, (480 - y - 240) * realDepth * xyScale, 5000.f);
			}

			dest++;
		}
	}

	return 0;
}

inline void drawLine(const Vector3 &v0, const Vector3 &v1)
{
	glBegin(GL_LINES);
	{
		glVertex3f(v0.getX(), v0.getY(), v0.getZ());
		glVertex3f(v1.getX(), v1.getY(), v1.getZ());
	}
	glEnd();
}

/// 스켈레톤 회전각 계산
void Kinect::refreshSingleSkeletonRotation(int parentBoneIndex, int boneIndex, const Matrix &localizeTransform, Matrix &resultLocalizeTransform)
{
	Vector3 &parentBone = skeleton[getKinectBoneIndex(parentBoneIndex)];
	Vector3 &bone = skeleton[getKinectBoneIndex(boneIndex)];
	if (
		(parentBone.getX() == 0.f && parentBone.getY() == 0.f && parentBone.getZ() == 0.f) ||
		(bone.getX() == 0.f && bone.getY() == 0.f && bone.getZ() == 0.f)
		)
	{	// 부모나 자신의 정보가 없으면 기본 각도로
		skeletonRotationInfo[getKinectBoneIndex(boneIndex)].set(0.f, 0.f, 0.f, 1.f);
		return;
	}

	Actor &base = ENGINE.getBaseActor();

	// 기본 벡터
	Vector3 baseVec, baseTemp;
	baseTemp = base.getBone(boneIndex)->getLocalEndPosition();
	localizeTransform.multiplyWithoutTranslate(baseVec, baseTemp);
	baseVec.normalize();

	// 현재 벡터
	Vector3 vec, temp;
	temp = skeleton[getKinectBoneIndex(boneIndex)] - skeleton[getKinectBoneIndex(parentBoneIndex)];
	//localizeTransform.multiplyWithoutTranslate(vec, temp);
	vec = temp;
	vec.normalize();

	// 각도 차이 구하기
	Quaternion q;
	q.setFromVectors(baseVec, vec);
	skeletonRotationInfo[getKinectBoneIndex(boneIndex)] = q;

	/*
	// 결과 지역화 변환행렬 구성
	Matrix rotMat, invRotMat, invTransMat;
	skeletonRotationInfo[getKinectBoneIndex(boneIndex)].getMatrix(rotMat);
	rotMat.getTranspose(invRotMat);
	invTransMat.setTranslate(base.getBone(parentBoneIndex)->getLocalPosition() - base.getBone(boneIndex)->getLocalPosition());
	resultLocalizeTransform.multiply(invRotMat, invTransMat);
	resultLocalizeTransform.multiply(localizeTransform);
	*/

	Matrix objectTransform, positionMat, rotationMat;
	positionMat.setTranslate(base.getBone(boneIndex)->getLocalEndPosition());
	q.getMatrix(rotationMat);
	objectTransform.multiply(rotationMat, positionMat);
    
	// 변환행렬 설정
	resultLocalizeTransform.multiply(localizeTransform, objectTransform);
}

// 스켈레톤 회전 정보 갱신
void Kinect::refreshSkeletonRotationInfo()
{
	Actor &actor = ENGINE.getActor();
	Actor &base = ENGINE.getBaseActor();

	Matrix hipCenterTrans;
	// Hip_Center
	{
		// 기본 법선
		const Vector3 &base_left = base.getBone(Bone::Hip_Left)->getLocalEndPosition();
		const Vector3 &base_right = base.getBone(Bone::Hip_Right)->getLocalEndPosition();
		Vector3 base_normal;
		base_normal.cross(base_right, base_left);
		base_normal.normalize();

		// 현재 법선
		const Vector3 &center = skeleton[getKinectBoneIndex(Bone::Hip_Center)];
		const Vector3 &left = skeleton[getKinectBoneIndex(Bone::Hip_Left)];
		const Vector3 &right = skeleton[getKinectBoneIndex(Bone::Hip_Right)];
		Vector3 normal;
		normal.cross(right - center, left - center);
		normal.normalize();

		// 둘간의 각도 차이 구하기
		skeletonRotationInfo[getKinectBoneIndex(Bone::Hip_Center)].setFromVectors(base_normal, normal);
		skeletonRotationInfo[getKinectBoneIndex(Bone::Hip_Center)].setIdentity();
		
		// 결과 지역화 변환행렬 구성
		Matrix rotMat;
		skeletonRotationInfo[getKinectBoneIndex(Bone::Hip_Center)].getMatrix(hipCenterTrans);
	}

	Matrix spineTrans, shoulderCenterTrans, headTrans;

	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Spine, hipCenterTrans, spineTrans);
	refreshSingleSkeletonRotation(Bone::Spine, Bone::Shoulder_Center, spineTrans, shoulderCenterTrans);
	refreshSingleSkeletonRotation(Bone::Shoulder_Center, Bone::Head, shoulderCenterTrans, headTrans);
	
	{
		Matrix shoulderTrans, elbowTrans, wristTrans, handTrans;
		refreshSingleSkeletonRotation(Bone::Shoulder_Center, Bone::Shoulder_Right, shoulderCenterTrans, shoulderTrans);
		refreshSingleSkeletonRotation(Bone::Shoulder_Right, Bone::Elbow_Right, shoulderTrans, elbowTrans);
		refreshSingleSkeletonRotation(Bone::Elbow_Right, Bone::Wrist_Right, elbowTrans, wristTrans);
		refreshSingleSkeletonRotation(Bone::Wrist_Right, Bone::Hand_Right, wristTrans, handTrans);
	}
	
	{
		Matrix shoulderTrans, elbowTrans, wristTrans, handTrans;
		refreshSingleSkeletonRotation(Bone::Shoulder_Center, Bone::Shoulder_Left, shoulderCenterTrans, shoulderTrans);
		refreshSingleSkeletonRotation(Bone::Shoulder_Left, Bone::Elbow_Left, shoulderTrans, elbowTrans);
		refreshSingleSkeletonRotation(Bone::Elbow_Left, Bone::Wrist_Left, elbowTrans, wristTrans);
		refreshSingleSkeletonRotation(Bone::Wrist_Left, Bone::Hand_Left, wristTrans, handTrans);
	}
	
	{
		Matrix hipTrans, kneeTrans, ankleTrans, footTrans;
		refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Hip_Right, hipCenterTrans, hipTrans);
		refreshSingleSkeletonRotation(Bone::Hip_Right, Bone::Knee_Right, hipTrans, kneeTrans);
		refreshSingleSkeletonRotation(Bone::Knee_Right, Bone::Ankle_Right, kneeTrans, ankleTrans);
		refreshSingleSkeletonRotation(Bone::Ankle_Right, Bone::Foot_Right, ankleTrans, footTrans);
	}
	
	{
		Matrix hipTrans, kneeTrans, ankleTrans, footTrans;
		refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Hip_Left, hipCenterTrans, hipTrans);
		refreshSingleSkeletonRotation(Bone::Hip_Left, Bone::Knee_Left, hipTrans, kneeTrans);
		refreshSingleSkeletonRotation(Bone::Knee_Left, Bone::Ankle_Left, kneeTrans, ankleTrans);
		refreshSingleSkeletonRotation(Bone::Ankle_Left, Bone::Foot_Left, ankleTrans, footTrans);
	}

}

/// 스켈레톤 정보 저장
void Kinect::saveSkeletonInfo()
{
	refreshSkeletonRotationInfo();

	FILE *fp = fopen("test.txt", "wb");

	for (uint i=0; i<skeletonRotationInfo.size(); ++i)
	{
		fprintf(fp, "%s (%lf,%lf,%lf,%lf)\n", ENGINE.getActor().getBone(i)->getName().c_str(),
			skeletonRotationInfo[i].getX(), skeletonRotationInfo[i].getY(), skeletonRotationInfo[i].getZ(), skeletonRotationInfo[i].getW());
	}

	fclose(fp);
}

/// 스켈레톤 그리기
void Kinect::drawSkeleton()
{
	glPointSize(5.f);
	glColor4ub(0, 255, 0, 255);
	
	glBegin(GL_POINTS);
	{
		for (int i=0; i<NUI_SKELETON_POSITION_COUNT; ++i)
		{
			glVertex3f(skeleton[i].getX(), skeleton[i].getY(), skeleton[i].getZ());
		}
	}
	glEnd();

	// Render Torso
	drawLine(skeleton[NUI_SKELETON_POSITION_HEAD], skeleton[NUI_SKELETON_POSITION_SHOULDER_CENTER]);
	drawLine(skeleton[NUI_SKELETON_POSITION_SHOULDER_CENTER], skeleton[NUI_SKELETON_POSITION_SHOULDER_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_SHOULDER_CENTER], skeleton[NUI_SKELETON_POSITION_SHOULDER_RIGHT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_SHOULDER_CENTER], skeleton[NUI_SKELETON_POSITION_SPINE]);
	drawLine(skeleton[NUI_SKELETON_POSITION_SPINE], skeleton[NUI_SKELETON_POSITION_HIP_CENTER]);
	drawLine(skeleton[NUI_SKELETON_POSITION_HIP_CENTER], skeleton[NUI_SKELETON_POSITION_HIP_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_HIP_CENTER], skeleton[NUI_SKELETON_POSITION_HIP_RIGHT]);

	// Left Arm
	drawLine(skeleton[NUI_SKELETON_POSITION_SHOULDER_LEFT], skeleton[NUI_SKELETON_POSITION_ELBOW_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_ELBOW_LEFT], skeleton[NUI_SKELETON_POSITION_WRIST_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_WRIST_LEFT], skeleton[NUI_SKELETON_POSITION_HAND_LEFT]);

	// Right Arm
	drawLine(skeleton[NUI_SKELETON_POSITION_SHOULDER_RIGHT], skeleton[NUI_SKELETON_POSITION_ELBOW_RIGHT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_ELBOW_RIGHT], skeleton[NUI_SKELETON_POSITION_WRIST_RIGHT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_WRIST_RIGHT], skeleton[NUI_SKELETON_POSITION_HAND_RIGHT]);

	// Left Leg
	drawLine(skeleton[NUI_SKELETON_POSITION_HIP_LEFT], skeleton[NUI_SKELETON_POSITION_KNEE_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_KNEE_LEFT], skeleton[NUI_SKELETON_POSITION_ANKLE_LEFT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_ANKLE_LEFT], skeleton[NUI_SKELETON_POSITION_FOOT_LEFT]);

	// Right Leg
	drawLine(skeleton[NUI_SKELETON_POSITION_HIP_RIGHT], skeleton[NUI_SKELETON_POSITION_KNEE_RIGHT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_KNEE_RIGHT], skeleton[NUI_SKELETON_POSITION_ANKLE_RIGHT]);
	drawLine(skeleton[NUI_SKELETON_POSITION_ANKLE_RIGHT], skeleton[NUI_SKELETON_POSITION_FOOT_RIGHT]);
}

/// 변환행렬 구성
bool Kinect::setTransformFromMarkerInfo(const MarkerInfo &markerInfo)
{
	// 변환행렬 구성위해 up, direction 벡터를 얻는다.
	Vector3 right, up, direction;
	up = markerInfo.v0 - markerInfo.v1;
	right = markerInfo.v2 - markerInfo.v1;
	direction.cross(up, right);
	up.normalize();
	direction.normalize();
	Vector3 vc;
	vc = (markerInfo.v0 + markerInfo.v1 + markerInfo.v2 + markerInfo.v3) / 4.f;
	
	// 변환행렬 구성
	transform.setViewMatrix(vc, direction, up);

	return true;
}

/// 포인트 클라우드에 변환행렬 적용
void Kinect::transformPointCloud(CloudElement *result, bool bgRemovalOption)
{
	if (bgRemovalOption && background)
	{
		// 마커 위치를 기준으로 pointCloud를 변환
		for (int i=0; i<depthWidth*depthHeight; ++i)
		{
			CloudElement &element = result[i];
			transform.multiply(element.position, pointCloud[i]);
			
			if (element.position.getZ() <= background[i].position.getZ() + 0.3f)
			{
				element.position.setZ(5000.f);
				continue;
			}
		
			element.color[0] = mappedColorBuffer[i*4 + 2];
			element.color[1] = mappedColorBuffer[i*4 + 1];
			element.color[2] = mappedColorBuffer[i*4 + 0];
		}
	}else
	{
		// 마커 위치를 기준으로 pointCloud를 변환
		for (int i=0; i<depthWidth*depthHeight; ++i)
		{
			CloudElement &element = result[i];
			transform.multiply(element.position, pointCloud[i]);
		
			element.color[0] = mappedColorBuffer[i*4 + 2];
			element.color[1] = mappedColorBuffer[i*4 + 1];
			element.color[2] = mappedColorBuffer[i*4 + 0];
		}
	}
}

/// 스켈레톤에 변환행렬 적용
void Kinect::transformSkeleton()
{
	// 마커 위치를 기준으로 skeleton을 변환
	for (int i=0; i<NUI_SKELETON_POSITION_COUNT; ++i)
	{
		Vector3 temp = skeleton[i];
		transform.multiply(skeleton[i], temp);
	}
}

static void drawTestMarkerRecognitionResult(Kinect *kinect, Kinect::MarkerInfo &marker)
{
//*
	ENGINE.clearPointCloud();

	/// 포인트 클라우드 변환, 추가
	int cloudSize = 640*480;
	CloudElement *cloud = new CloudElement[cloudSize];

	// 변환
	kinect->setTransformFromMarkerInfo(marker);
	kinect->transformPointCloud(cloud);

	// 포인트 클라우드 큐에 넣어놓기
	ENGINE.addPointCloud(cloud, cloudSize);
	
	// 그리기
	ENGINE.draw();
    SwapBuffers(hDC);
//*/
}

static void computeAverageMarkerInfo(const vector<Kinect::MarkerInfo> &markers, Kinect::MarkerInfo &result)
{
	result.v0.set(0, 0, 0);
	result.v1.set(0, 0, 0);
	result.v2.set(0, 0, 0);
	result.v3.set(0, 0, 0);

	// 일단 값을 모두 더한다
	for (uint i=0; i<markers.size(); ++i)
	{
		const Kinect::MarkerInfo &marker = markers[i];
		result.v0 += marker.v0;
		result.v1 += marker.v1;
		result.v2 += marker.v2;
		result.v3 += marker.v3;
	}

	// 개수로 나눈다
	result.v0 /= markers.size();
	result.v1 /= markers.size();
	result.v2 /= markers.size();
	result.v3 /= markers.size();
}

// 여러번 반복하여 정확하게 마커 인식
bool Kinect::recognizeMakerAccurately(int repeatCount, int limitDeadCount)
{
	vector<MarkerInfo> markers;

	// 회수만큼 반복
	int deadCount = 0;
	for (int i=0; i<repeatCount;)
	{
		if (deadCount >= limitDeadCount)
			return false;

		// depth와 color 버퍼를 갱신하고 매핑한다.
		if (refreshDepthAndColorBuffer() == false)
		{
			++deadCount;
			continue;
		}

		// 마커 인식
		MarkerRecognizer::sMarkerInfo marker;
		if (recognizeMarker(marker) == false)
		{
			++deadCount;
			continue;
		}

		// 마커 정보 변환
		markers.push_back(MarkerInfo());
		if (convertMarkerInfo(marker, markers.back()) == false)
		{
			++deadCount;
			markers.pop_back();
			continue;
		}

		// for test
		drawTestMarkerRecognitionResult(this, markers.back());

		++i;
	}

	// 마커들의 값을 평균낸다
	MarkerInfo averageMarker;
	computeAverageMarkerInfo(markers, averageMarker);

	// 마커 위치를 기준으로 변환행렬 구성
	bool transformed = setTransformFromMarkerInfo(averageMarker);
	if (transformed == false)
	{	// 변환 실패
		return false;
	}
	
	// for test
	drawTestMarkerRecognitionResult(this, averageMarker);

	return true;
}

bool Kinect::refreshDepthAndColorBuffer()
{
	/// depth와 color 버퍼를 갱신하고 매핑한다.
	bool depthIsRefreshed, colorIsRefreshed;
	do
		depthIsRefreshed = refreshDepthBuffer() >= 0;
	while(depthIsRefreshed == false);
	do
		colorIsRefreshed = refreshColorBuffer() >= 0;
	while (colorIsRefreshed == false);

	if (depthIsRefreshed && colorIsRefreshed)
	{
		mapColorToDepth();
		return true;

	}else
	{
		return false;
	}
}

bool Kinect::recognizeMarker(MarkerRecognizer::sMarkerInfo &marker)
{
	MARKER_RECOGNIZER.recognizeMarker(getColorWidth(), getColorHeight(), getMappedColorBuffer());

	// 마커가 없으면 실패
	int count = MARKER_RECOGNIZER.getMarkerCount();
	if (count <= 0)
		return false;

	marker = MARKER_RECOGNIZER.getMarker(0);

	return true;
}
bool Kinect::isValidMarker(const MarkerRecognizer::sMarkerInfo &marker)
{
	// 코너를 얻어옴
	const Vector3 &v0 = pointCloud[(int)marker.corner[0].x + (int)marker.corner[0].y * depthWidth];
	const Vector3 &v1 = pointCloud[(int)marker.corner[1].x + (int)marker.corner[1].y * depthWidth];
	const Vector3 &v2 = pointCloud[(int)marker.corner[2].x + (int)marker.corner[2].y * depthWidth];
	const Vector3 &v3 = pointCloud[(int)marker.corner[3].x + (int)marker.corner[3].y * depthWidth];

	if (v0.getZ() > 4000 ||
		v1.getZ() > 4000 ||
		v2.getZ() > 4000 ||
		v3.getZ() > 4000)
	{
		return false;
	}

	return true;
}

bool Kinect::convertMarkerInfo(const MarkerRecognizer::sMarkerInfo &from, MarkerInfo &to)
{
	if (isValidMarker(from) == false)
		return false;

	// 코너를 얻어옴
	to.v0 = pointCloud[(int)from.corner[0].x + (int)from.corner[0].y * depthWidth];
	to.v1 = pointCloud[(int)from.corner[1].x + (int)from.corner[1].y * depthWidth];
	to.v2 = pointCloud[(int)from.corner[2].x + (int)from.corner[2].y * depthWidth];
	to.v3 = pointCloud[(int)from.corner[3].x + (int)from.corner[3].y * depthWidth];

	return true;
}

void Kinect::addBackground(CloudElement *bg)
{
	// 기존 값이 null 이면
	if (background == null)
	{
		background = bg;
		return;
	}

	// 기존 값과 비교하여 더 적합한 값이 있다면 수정
	int size = 640*480;
	for (int i=0; i<size; ++i)
	{

		if (background[i].position.getZ() < bg[i].position.getZ() + 0.3f)
		{
			background[i].position = bg[i].position;
		}
	}

	DELETE_ARRAY(bg);
}