// opencv_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MarkerRecognizer.h"


using namespace std;

MarkerRecognizer::MarkerRecognizer() : MARKER_WIDTH(0.15f), MARKER_HEIGHT(0.15f)
{
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .4, .4, 0, 1, 8);
	calib.LoadCalibParams(cvSize(640, 480));
}

MarkerRecognizer::~MarkerRecognizer()
{
}

/// ��Ŀ�� ��ġ�� ��´�.
void MarkerRecognizer::recognizeMarker(int width, int height, byte *buffer)
{
	const int bytePerPixel = 4;
	CvSize imageSize;
	IplImage *image;

	// �ʱ�ȭ
	markers.clear();

	// �̹��� ����
	imageSize.width = width;
	imageSize.height = height;
	image = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_8U, 4);
	cvSetData(image, buffer, image->widthStep);

	// gray ��ȯ
	IplImage *grayImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCvtColor(image, grayImage, CV_RGB2GRAY);

	// ����ȭ
	IplImage *binaryImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCopyImage(grayImage, binaryImage);
	cvAdaptiveThreshold(binaryImage, binaryImage, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 71, 15);

	// ������ ������ �޸� �Ҵ�
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *contours = NULL;

	// ������ ã��
	int contourCount = cvFindContours(binaryImage, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	// �ٻ�ȭ
	CvSeq *approxContours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 1, 1);

	// ������ ���
	//cvDrawContours (image, approxContours, CV_RGB(255,255,0), CV_RGB(0,255,0), 10, 1, CV_AA);

	// ��Ŀ ã��
	findMarker(approxContours, storage, 0);
	getMarkerCode(grayImage, image);
	
	// release
	cvReleaseMemStorage(&storage);
	cvReleaseImageHeader(&image);
	cvReleaseImage(&grayImage);
	cvReleaseImage(&binaryImage);
}

void getInverseMatrix(float src[][2], float dst[][2])
{
	float det = src[0][0] * src[1][1] - src[0][1] * src[1][0];

	dst[0][0] = src[1][1] / det;
	dst[0][1] = - src[0][1] / det;
	dst[1][0] = - src[1][0] / det;
	dst[1][1] = src[0][0] / det;
}

bool checkRectCenter(CvSeq *seq) 
{
	CvPoint corner[4] = {
		*(CvPoint *)cvGetSeqElem(seq, 0),
		*(CvPoint *)cvGetSeqElem(seq, 1),
		*(CvPoint *)cvGetSeqElem(seq, 2),
		*(CvPoint *)cvGetSeqElem(seq, 3),
	};

	// �� �簢���� �� �밢�� ������ �簢�� �߾ӿ� ������ �˻��Ѵ�.
	// ���⼭�� �� ���� l1, l2�� �������� t�� u ���� ����ϴ� ���̴�.
	// l1 = (a1,b1) + t*(x1,y1)
	// l2 = (a2,b2) + u*(x2,y2)

	double a1 = corner[0].x;
	double b1 = corner[0].y;
	double x1 = corner[2].x - corner[0].x;
	double y1 = corner[2].y - corner[0].y;

	double a2 = corner[1].x;
	double b2 = corner[1].y;
	double x2 = corner[3].x - corner[1].x;
	double y2 = corner[3].y - corner[1].y;

	// ���� ���� ������ ���� ����: ������ �����İ� ����.docx
    float A[2][2];
    A[0][0] =  2*(x1*x1 + y1*y1);
    A[0][1] = -2*(x1*x2 + y1*y2);
    A[1][0] = -2*(x1*x2 + y1*y2);
    A[1][1] =  2*(x2*x2 + y2*y2);

	// �����
	float inverseA[2][2];
	getInverseMatrix(A, inverseA);

 	float B[2][1];
	B[0][0] =  2*(x1*(a2 - a1) + y1*(b2 - b1));
	B[1][0] = -2*(x2*(a2 - a1) + y2*(b2 - b1));

	// t, u ���
	float x[2][1];
	x[0][0] = inverseA[0][0] * B[0][0] + inverseA[0][1] * B[1][0];
	x[1][0] = inverseA[1][0] * B[0][0] + inverseA[1][1] * B[1][0];

	// t, u ���� 0.5�� �������� +-0.15 ���̿� ������ �˻��Ѵ�.
	const double l_th = 0.15;
	if (fabs(x[0][0] - 0.5) < l_th && fabs(x[1][0] - 0.5) < l_th) {
		// ����
		return true;
	}
	else return false;
}

void MarkerRecognizer::findMarker(CvSeq *contours, CvMemStorage *storage, int level)
{
	for (CvSeq *s = contours; s; s = s->h_next)
	{
		// ¦����° ����� ó���ϱ� ���� level%2 == 0�� �˻��Ѵ�. 
		// �� �������� �ѷ��� ����� ó���ϱ� ���ؼ���
		// �׸��� ����� �����ϴ� ���� ���� 4�� �̻� �Ǿ�� �簢�� �ĺ��� �ȴ�.
		if (level%2 == 0 && s->total >= 4)
		{
			// �ٿ�� �ڽ��� ã�� ������ �������� �뷫���� ũ�⸦ �˱� ���ؼ���.
			// ũ�⿡ ���� ����� approximation �ϴ� ���е��� �����Ѵ�.
			// ���⼭�� �뷫 10%������ ���е��� �����Ѵ�. (d*approx_param �κ�)
			CvRect rect = cvBoundingRect (s);

			double d = sqrt ((double)rect.height*rect.width);

			const double d_th = 12.;
			const double approx_param = 0.1;

			// �������� �뷫���� ũ�Ⱑ d_th���� Ŀ�� �Ѵ�.
			if (d > d_th) {
				CvSeq *ss = cvApproxPoly (s, s->header_size, storage, CV_POLY_APPROX_DP, d*approx_param, 0);
				// ����� approximation �ϰ��� �ڳ��� ���� 4��(�簢��)���� �˻��Ѵ�.
				if (ss->total == 4) {
					// �߰�������, �� �簢���� �� �밢�� ������ �簢�� �߾ӿ� ������ �˻��Ѵ�.
					if (checkRectCenter(ss))
					{
						// ��Ŀ�� ã�Ҵ�. ��Ŀ ��Ͽ� �����Ѵ�.
						sMarkerInfo mi;

						mi.width = MARKER_WIDTH;		// ���� ��Ŀ�� ���� ���� (����: m)
						mi.height = MARKER_HEIGHT;		// ���� ��Ŀ�� ���� ���� (����: m)
						mi.ID = -1;						// -1�� �ʱ�ȭ
						mi.corner[0] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(ss, 0));
						mi.corner[1] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(ss, 1));
						mi.corner[2] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(ss, 2));
						mi.corner[3] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(ss, 3));
						
						markers.push_back (mi);
					}
				}
			}
		}

		if (s->v_next) {
			findMarker (s->v_next, storage, level+1);
		}
	}
}

void MarkerRecognizer::getMarkerCode(IplImage *src, IplImage *dst)
{
	bool invalid = false;	// �ùٸ� ��Ŀ�� �ƴϸ� true
	for (unsigned int i=0; i<markers.size(); ++i)
	{
		invalid = false;

		// ����� ��Ŀ�� �ڳʷκ��� �����ȼ� ��Ȯ���� �ڳ� ��ǥ�� �ٽ� ���Ѵ�.
		cvFindCornerSubPix (src, markers[i].corner, 4, cvSize(2, 2), cvSize(-1, -1), 
			cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.01));

		// src���� ã�� ��Ŀ�� �������κ��� ��Ŀ ������ �����Ѵ�.
		const int marker_size = 60;
		IplImage *img_marker = cvCreateImage (cvSize (marker_size, marker_size), IPL_DEPTH_8U, 1);
		extractMarkerImage (src, img_marker, markers[i]);

		// ��Ŀ�� 6 x 6�� ��ķ� �����ȴ�.
		double code_matrix[6][6] = {0, };

		// ��Ŀ ������ �ȼ����� �����κ��� �ڵ� ���� �����Ѵ�.
		extractCodeFromImage (img_marker, code_matrix);

		if (checkParity (code_matrix)) {
			int rotate_index = getRotation (code_matrix);
			if (0 <= rotate_index) {
				// ��Ŀ �ν� ����!!!

				// ��Ŀ�� �ڵ带 ������ ����� ȸ���� ������ ������ �ش�.
				rotateMatrix (code_matrix, rotate_index);
				rotateCorner (markers[i].corner, rotate_index);

				markers[i].ID = calcMarkerID (code_matrix);
				// TRACE ("Marker ID = %d\n", markers[i].ID);
				
				findMarkerPos3d (&markers[i]);

				drawMarkerAxis (&markers[i], dst);

				// ���� ��Ŀ �ڵ�
				// cvNamedWindow ("Marker Image Org", CV_WINDOW_AUTOSIZE);
				// cvShowImage ("Marker Image Org", img_marker);

				// showMarkerCode (cvGetSize(img_marker), code_matrix);
			}else
				invalid = true;
		}else
			invalid = true;

		if (invalid)
		{	// �ùٸ� ��Ŀ�� �ƴ�
			// ����
			markers[i] = markers[markers.size()-1];
			markers.resize(markers.size()-1);
			--i;
			
			cvReleaseImage (&img_marker);
			continue;
		}

		cvReleaseImage (&img_marker);
	}
}

void MarkerRecognizer::extractMarkerImage (IplImage *src, IplImage *dst, sMarkerInfo &mi)
{
	assert (src->nChannels == 1);
	assert (dst->width == dst->height);

	const float ignoring_margin = 0.f;	// ���� �̹����κ��� ��Ŀ �̹����� �����ϸ鼭 ������ �׵θ��� ����

	// ������ ��Ŀ�� ������ �̹��� ���� ��ǥ
	CvPoint2D32f dest_corner[4] =
	{ 
		{ -ignoring_margin,				-ignoring_margin}, 
		{ -ignoring_margin,				dst->height + ignoring_margin}, 
		{ dst->width + ignoring_margin,	dst->height + ignoring_margin}, 
		{ dst->width + ignoring_margin,	-ignoring_margin} 
	};

	CvMat *transform_matrix = cvCreateMat(3, 3, CV_32FC1);

	// �ҽ� �̹������� ��Ŀ�� �ڳʿ� ���� ������ ��Ŀ �̹��� ���� ����� �����ϱ� ���� ��ȯ ����� ���Ѵ�.
	cvGetPerspectiveTransform (mi.corner, dest_corner, transform_matrix);

	// �ҽ� �̹��� ���� ��Ŀ�� ��Ŀ �̹����� �����Ѵ�.
	cvWarpPerspective (src, dst, transform_matrix);

	cvReleaseMat (&transform_matrix);
}

void MarkerRecognizer::extractCodeFromImage (IplImage *src, double code_matrix[6][6])
{
	#define PIXEL_YX(img,y,x)	(unsigned char &)img->imageData[(y)*img->widthStep + (x)]

	assert (src->width == 60 && src->height == 60);

	// ��Ŀ �̹����� 6x6 ���ڷ� �ɰ� �� ������ ���� ���� �ȼ����� ��� ���Ѵ�.
	for (int y = 0; y < src->height; y++) {
		for (int x = 0; x < src->width; x++) {
			int yi = y/10;
			int xi = x/10;

			assert (yi < 6 && xi < 6);
			code_matrix[yi][xi] += PIXEL_YX(src, y, x);
		}
	}

	double min_v = 255.; 
	double max_v = 0.;
	
	// ���� ���� 0 ~ 1 ������ ������ ����ȭ �ϸ鼭 �ִ밪�� �ּҰ��� ã�´�.
	// �ϳ��� ���ڿ��� 100���� �ȼ��� �������� �� �ȼ��� �ִ� ���� 255�̱� ������
	// ���� ���� 100*255�� �������ָ� �ȴ�.
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			code_matrix[y][x] /= 100.*255;

			if (min_v > code_matrix[y][x]) min_v = code_matrix[y][x];
			if (max_v < code_matrix[y][x]) max_v = code_matrix[y][x];
		}
	}

	// �ִ밪�� �ּҰ��� �߰����� ã�´�.
	double mid_v = (min_v + max_v)/2.;

	// �߰����� �������� �������� ������ 1.�� ����� ������ 0.�� �����Ѵ�.
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			code_matrix[y][x] = (code_matrix[y][x] < mid_v) ? 1. : 0.;
		}
	}
}

bool MarkerRecognizer::checkParity (double code_matrix[6][6])
{
	int sum = 0;

	// �׵θ��� ��� ����� �ִ��� �˻��Ѵ�.
	// ��, �� ������ �� ���� 6���̰� �𼭸��� 4���̴ϱ� 
	// ���� 24���� �Ǿ�� �Ѵ�.
	for (int i = 0; i < 6; i++) {
		sum += (int)code_matrix[0][i];
		sum += (int)code_matrix[5][i];
		sum += (int)code_matrix[i][0];
		sum += (int)code_matrix[i][5];
	}
	if (sum != 24) return false;

	sum = 0;

	// üũ���� �˻��Ѵ�.
	// �׵θ��� ������ ���� ���� ���� 1���� �Ǿ�� �Ѵ�.
	for (int y = 1; y < 5; y++) {
		for (int x = 1; x < 5; x++) {
			sum += (int)code_matrix[y][x];
		}
	}
	return (sum == 1);
}

int MarkerRecognizer::getRotation (double code_matrix[6][6])
{
	if      (code_matrix[1][1] && !code_matrix[1][4] && !code_matrix[4][4] && !code_matrix[4][1]) return 0;	// ����
	else if (!code_matrix[1][1] && code_matrix[1][4] && !code_matrix[4][4] && !code_matrix[4][1]) return 1;	// �ð�������� 90�� ȸ����
	else if (!code_matrix[1][1] && !code_matrix[1][4] && code_matrix[4][4] && !code_matrix[4][1]) return 2; // �ð�������� 180�� ȸ����
	else if (!code_matrix[1][1] && !code_matrix[1][4] && !code_matrix[4][4] && code_matrix[4][1]) return 3; // �ð�������� 270�� ȸ����
	else return -1; // ������ ���� �����̴�. ����
}

void MarkerRecognizer::rotateMatrix (double code_matrix[6][6], int angle_idx)
{
	if (angle_idx == 0) return;

	double cb[6][6];

	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			switch (angle_idx) {
			case 1: cb[y][x] = code_matrix[x][5-y];		break; // �ݽð� �������� 90�� ȸ��
			case 2: cb[y][x] = code_matrix[5-y][5-x];	break; // �ݽð� �������� 180�� ȸ��
			case 3: cb[y][x] = code_matrix[5-x][y];		break; // �ݽð� �������� 270�� ȸ��
			}
		}
	}
	memcpy (code_matrix, cb, sizeof(double)*6*6);
}

void MarkerRecognizer::rotateCorner (CvPoint2D32f corner[4], int angle_idx)
{
	if (angle_idx == 0) return;

	CvPoint2D32f c[4];

	for (int i=0; i<4; ++i) {
		c[i] = corner[(i + 4 - angle_idx)%4];
	}
	memcpy (corner, c, sizeof(CvPoint2D32f)*4);
}

int MarkerRecognizer::calcMarkerID (double code_matrix[6][6])
{
	int id = 0;
	if (code_matrix[4][2]) id += 1;
	if (code_matrix[3][4]) id += 2;
	if (code_matrix[3][3]) id += 4;
	if (code_matrix[3][2]) id += 8;
	if (code_matrix[3][1]) id += 16;
	if (code_matrix[2][4]) id += 32;
	if (code_matrix[2][3]) id += 64;
	if (code_matrix[2][2]) id += 128;
	if (code_matrix[2][1]) id += 256;
	if (code_matrix[1][3]) id += 512;
	if (code_matrix[1][2]) id += 1024;

	return id;
}

void MarkerRecognizer::findMarkerPos3d (sMarkerInfo *marker)
{
	if (calib._intrinsic_matrix && calib._distortion_coeffs);
	else return;

	// ȸ��(rotation)�� �̵�(translation)�� ����Ͽ� ������ ��Ʈ���� ����
	CvMat rotation    = cvMat(3, 1, CV_32FC1, marker->rotation);
	CvMat translation = cvMat(3, 1, CV_32FC1, marker->translation);

	float image_xy[4][2] = {
		{ marker->corner[0].x, marker->corner[0].y },
		{ marker->corner[1].x, marker->corner[1].y },
		{ marker->corner[2].x, marker->corner[2].y },
		{ marker->corner[3].x, marker->corner[3].y },
	};

	float object_xyz[4][3] = {
		{ 0.0f,				0.0f,			0.0f },
		{ 0.0f,				marker->height,	0.0f },
		{ marker->width,	marker->height,	0.0f },
		{ marker->width,	0.0f,			0.0f },
	};

	CvMat image_points  = cvMat(4, 2, CV_32FC1, &image_xy[0][0]);
	CvMat object_points = cvMat(4, 3, CV_32FC1, &object_xyz[0][0]);
	
	// 3���� �������� ��Ŀ�� ��ġ�� ������ ã�´�.
	cvFindExtrinsicCameraParams2 (&object_points, &image_points, 
		calib._intrinsic_matrix, calib._distortion_coeffs, 
		&rotation, &translation);
}

void MarkerRecognizer::showMarkerCode (CvSize &size, double code_matrix[6][6])
{
	// �ڵ� ������κ��� ���� ��Ŀ �ڵ带 �̹����� ��ȯ�Ͽ� ǥ���Ѵ�.

	IplImage* img = cvCreateImage (size, IPL_DEPTH_8U, 1);

	cvSet (img, CV_RGB(255,255,255));

	double dx = img->width/6.;
	double dy = img->height/6.;

	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			if (code_matrix[y][x]) {
				cvDrawRect(img, cvPoint(cvRound(x*dx), cvRound(y*dy)), 
					cvPoint (cvRound((x+1)*dx), cvRound((y+1)*dy)), CV_RGB(0,0,0), CV_FILLED);
			}
		}
	}

	cvNamedWindow ("Marker Image", CV_WINDOW_AUTOSIZE);
	cvShowImage ("Marker Image", img);

	cvReleaseImage(&img);
}

void MarkerRecognizer::drawMarkerAxis (sMarkerInfo *marker, IplImage *dst)
{
	float depth = max(marker->width, marker->height);

	// 3���� �������� ��Ŀ�� x, y, z ��ǥ�� �����Ѵ�.
	float object_xyz[4][3] = {
		{0.f,			0.f,			0.f		},
		{marker->width,	0.f,			0.f		},
		{0.f,			marker->height,	0.f		},
		{0.f,			0.f,			depth	},
	};
	float image_xy[4][2] = { 0.f, };

	CvMat image_points  = cvMat(4, 2, CV_32FC1, &image_xy[0][0]);
	CvMat object_points = cvMat(4, 3, CV_32FC1, &object_xyz[0][0]);

	CvMat rotation    = cvMat(3, 1, CV_32FC1, marker->rotation);
	CvMat translation = cvMat(3, 1, CV_32FC1, marker->translation);

	// ��Ŀ�� x, y, z ��ǥ�� �̹����� �������� �Ѵ�.
	cvProjectPoints2 (&object_points, &rotation, &translation, calib._intrinsic_matrix, calib._distortion_coeffs, &image_points);

	// 2�������� �������ǵ� ��ǥ�� �׸���.
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[1][0]), cvRound(image_xy[1][1])), CV_RGB(255,0,0), 2);
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[2][0]), cvRound(image_xy[2][1])), CV_RGB(0,255,0), 2);
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[3][0]), cvRound(image_xy[3][1])), CV_RGB(0,0,255), 2);

	// ��Ŀ�� ID�� ǥ���Ѵ�.
	char buff[256];
	sprintf (buff, "     ID: %d", marker->ID);
	cvPutText(dst, buff, cvPointFrom32f(marker->corner[0]), &font, CV_RGB(255, 0, 0));
}