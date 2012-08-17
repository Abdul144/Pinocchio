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
		float width, height;	// ��Ŀ�� ũ��
		CvPoint2D32f center;	// ��Ŀ�� �߽� ��
		CvPoint2D32f corner[4];	// �簢���� �� �ڳ� ��
		int ID;					// ��Ŀ���� �νĵ� ID
	
		float rotation[3];		// ��Ŀ�� ȸ���� ��Ÿ���� ���� (3 x 1)
		float translation[3];	// ��Ŀ�� �̵��� ��Ÿ���� ���� (3 x 1)
};

	// ��� ����
private:
	const float MARKER_WIDTH;
	const float MARKER_HEIGHT;
	
	vector<sMarkerInfo> markers;
	CvFont font;
	CamCalib calib;


	// ��� �Լ�
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
	/// ��Ŀ�� ��ġ�� ��´�.
	void recognizeMarker(int width, int height, byte *buffer);

	inline uint getMarkerCount()
	{
		return markers.size();
	}

	inline sMarkerInfo&  getMarker(int index)
	{
		return markers[index];
	}

	
	// ����
public:
};