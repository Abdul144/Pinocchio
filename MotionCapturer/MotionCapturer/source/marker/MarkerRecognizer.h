#pragma once

#include <opencv/cv.h>
#include <vector>

#include "../util/Singleton.h"
#include "CamCalib.h"

#define MARKER_RECOGNIZER		MarkerRecognizer::getInstance()

using namespace std;

class MarkerRecognizer : public Singleton<MarkerRecognizer>
{
	USE_SINGLETON(MarkerRecognizer)
public:	
	struct sMarkerInfo 
	{
		float width, height;	// 마커의 크기
		CvPoint2D32f center;	// 마커의 중심 점
		CvPoint2D32f corner[4];	// 사각형의 네 코너 점
		int ID;					// 마커에서 인식된 ID
	
		float rotation[3];		// 마커의 회전을 나타내는 벡터 (3 x 1)
		float translation[3];	// 마커의 이동을 나타내는 벡터 (3 x 1)
};

	// 멤버 변수
private:
	const float MARKER_WIDTH;
	const float MARKER_HEIGHT;
	
	vector<sMarkerInfo> markers;
	CvFont font;
	CamCalib calib;


	// 멤버 함수
private:
	MarkerRecognizer();
	~MarkerRecognizer();

	void findMarker(CvSeq *contours, CvMemStorage *storage, int level);
	void getMarkerCode(IplImage *src, IplImage *dst);
	void extractMarkerImage (IplImage *src, IplImage *dst, sMarkerInfo &mi);
	void extractCodeFromImage (IplImage *src, double code_matrix[6][6]);
	bool checkParity (double code_matrix[6][6]);
	int getRotation (double code_matrix[6][6]);
	void rotateMatrix (double code_matrix[6][6], int angle_idx);
	void rotateCorner (CvPoint2D32f corner[4], int angle_idx);
	int calcMarkerID (double code_matrix[6][6]);
	void findMarkerPos3d (sMarkerInfo *marker);
	void showMarkerCode (CvSize &size, double code_matrix[6][6]);
	void drawMarkerAxis (sMarkerInfo *marker, IplImage *dst);

public:
	/// 마커의 위치를 얻는다.
	void recognizeMarker(int width, int height, byte *buffer);

	inline uint getMarkerCount()
	{
		return markers.size();
	}

	inline sMarkerInfo&  getMarker(int index)
	{
		return markers[index];
	}

	
	// 접근
public:
};