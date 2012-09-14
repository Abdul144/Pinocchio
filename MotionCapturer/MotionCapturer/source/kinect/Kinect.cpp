
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


Kinect::Kinect(INuiSensor *sensor, bool useSkeleton) :
		sensor(sensor), 
		nextDepthFrameEvent(null),
		depthStreamHandle(null),
		nextColorFrameEvent(null),
		colorStreamHandle(null),
		nextSkeletonFrameEvent(null),
		skeletonTrakingFlags(0)
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

	// 변환행렬 초기화
	transform.setIdentity();

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
void Kinect::refreshSingleSkeletonRotation(int parentBoneIndex, int boneIndex)
{
	if (
		(skeleton[parentBoneIndex].getX() == 0.f && skeleton[parentBoneIndex].getY() == 0.f && skeleton[parentBoneIndex].getZ() == 0.f) ||
		(skeleton[boneIndex].getX() == 0.f && skeleton[boneIndex].getY() == 0.f && skeleton[boneIndex].getZ() == 0.f)
		)
	{	// 부모나 자신의 정보가 없으면 기본 각도로
		skeletonRotationInfo[boneIndex].set(0.f, 0.f, 0.f, 1.f);
		return;
	}

	Actor &base = ENGINE.getBaseActor();

	// 기본 벡터
	Vector3 baseVec;
	baseVec = base.getBone(boneIndex)->getLocalPosition() - base.getBone(parentBoneIndex)->getLocalPosition();
	baseVec.normalize();

	// 현재 벡터
	Vector3 vec;
	vec = skeleton[boneIndex] - skeleton[parentBoneIndex];
	vec.normalize();

	// 각도 차이 구하기
	skeletonRotationInfo[boneIndex].setFromVectors(baseVec, vec);
}

// 스켈레톤 회전 정보 갱신
void Kinect::refreshSkeletonRotationInfo()
{
	/// 기본 각도와의 차이 계산
	Actor &actor = ENGINE.getActor();
	Actor &base = ENGINE.getBaseActor();

	// Hip_Center
	{
		// 기본 법선
		const Vector3 &base_center = base.getBone(Bone::Hip_Center)->getLocalPosition();
		const Vector3 &base_left = base.getBone(Bone::Hip_Left)->getLocalPosition();
		const Vector3 &base_right = base.getBone(Bone::Hip_Right)->getLocalPosition();
		Vector3 base_normal;
		base_normal.cross(base_right - base_center, base_left - base_center);
		base_normal.normalize();

		// 현재 법선
		const Vector3 &center = skeleton[Bone::Hip_Center];
		const Vector3 &left = skeleton[Bone::Hip_Left];
		const Vector3 &right = skeleton[Bone::Hip_Right];
		Vector3 normal;
		normal.cross(right - center, left - center);
		normal.normalize();

		// 둘간의 각도 차이 구하기
		skeletonRotationInfo[Bone::Hip_Center].setFromVectors(base_normal, normal);
	}

	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Spine);
	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Shoulder_Center);
	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Head);
    
	refreshSingleSkeletonRotation(Bone::Shoulder_Center, Bone::Shoulder_Right);
	refreshSingleSkeletonRotation(Bone::Shoulder_Right, Bone::Elbow_Right);
	refreshSingleSkeletonRotation(Bone::Elbow_Right, Bone::Wrist_Right);
	refreshSingleSkeletonRotation(Bone::Wrist_Right, Bone::Hand_Right);
	
	refreshSingleSkeletonRotation(Bone::Shoulder_Center, Bone::Shoulder_Left);
	refreshSingleSkeletonRotation(Bone::Shoulder_Left, Bone::Elbow_Left);
	refreshSingleSkeletonRotation(Bone::Elbow_Left, Bone::Wrist_Left);
	refreshSingleSkeletonRotation(Bone::Wrist_Left, Bone::Hand_Left);
	
	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Hip_Right);
	refreshSingleSkeletonRotation(Bone::Hip_Right, Bone::Knee_Right);
	refreshSingleSkeletonRotation(Bone::Knee_Right, Bone::Ankle_Right);
	refreshSingleSkeletonRotation(Bone::Ankle_Right, Bone::Foot_Right);
	
	refreshSingleSkeletonRotation(Bone::Hip_Center, Bone::Hip_Left);
	refreshSingleSkeletonRotation(Bone::Hip_Left, Bone::Knee_Left);
	refreshSingleSkeletonRotation(Bone::Knee_Left, Bone::Ankle_Left);
	refreshSingleSkeletonRotation(Bone::Ankle_Left, Bone::Foot_Left);

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
bool Kinect::setTransform(MarkerRecognizer::sMarkerInfo &marker)
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

	// 변환행렬 구성위해 up, direction 벡터를 얻는다.
	Vector3 right, up, direction;
	up = v0 - v1;
	right = v2 - v1;
	direction.cross(up, right);
	up.normalize();
	direction.normalize();
	Vector3 vc;
	vc = (v0 + v1 + v2 + v3) / 4.f;
	
	// 변환행렬 구성
	transform.setViewMatrix(vc, direction, up);

	return true;
}

/// 포인트 클라우드에 변환행렬 적용
void Kinect::transformPointCloud(CloudElement *result)
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