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

/// 마커의 위치를 얻는다.
void MarkerRecognizer::recognizeMarker(int width, int height, byte *buffer)
{
	const int bytePerPixel = 4;
	CvSize imageSize;
	IplImage *image;

	// 초기화
	markers.clear();

	// 이미지 생성
	imageSize.width = width;
	imageSize.height = height;
	image = cvCreateImageHeader(cvSize(640, 480), IPL_DEPTH_8U, 4);
	cvSetData(image, buffer, image->widthStep);

	// gray 변환
	IplImage *grayImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCvtColor(image, grayImage, CV_RGB2GRAY);

	// 이진화
	IplImage *binaryImage = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCopyImage(grayImage, binaryImage);
	cvAdaptiveThreshold(binaryImage, binaryImage, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 71, 15);

	// 윤곽을 저장할 메모리 할당
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *contours = NULL;

	// 윤곽선 찾기
	int contourCount = cvFindContours(binaryImage, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	// 근사화
	CvSeq *approxContours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 1, 1);

	// 윤곽선 출력
	//cvDrawContours (image, approxContours, CV_RGB(255,255,0), CV_RGB(0,255,0), 10, 1, CV_AA);

	// 마커 찾기
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

	// 이 사각형의 두 대각선 교점이 사각형 중앙에 오는지 검사한다.
	// 여기서는 두 라인 l1, l2의 교점에서 t와 u 값을 계산하는 식이다.
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

	// 관련 식의 유도는 문서 참조: 직선의 방정식과 교점.docx
    float A[2][2];
    A[0][0] =  2*(x1*x1 + y1*y1);
    A[0][1] = -2*(x1*x2 + y1*y2);
    A[1][0] = -2*(x1*x2 + y1*y2);
    A[1][1] =  2*(x2*x2 + y2*y2);

	// 역행렬
	float inverseA[2][2];
	getInverseMatrix(A, inverseA);

 	float B[2][1];
	B[0][0] =  2*(x1*(a2 - a1) + y1*(b2 - b1));
	B[1][0] = -2*(x2*(a2 - a1) + y2*(b2 - b1));

	// t, u 계산
	float x[2][1];
	x[0][0] = inverseA[0][0] * B[0][0] + inverseA[0][1] * B[1][0];
	x[1][0] = inverseA[1][0] * B[0][0] + inverseA[1][1] * B[1][0];

	// t, u 값이 0.5를 기준으로 +-0.15 사이에 오는지 검사한다.
	const double l_th = 0.15;
	if (fabs(x[0][0] - 0.5) < l_th && fabs(x[1][0] - 0.5) < l_th) {
		// 성공
		return true;
	}
	else return false;
}

void MarkerRecognizer::findMarker(CvSeq *contours, CvMemStorage *storage, int level)
{
	for (CvSeq *s = contours; s; s = s->h_next)
	{
		// 짝수번째 컨투어만 처리하기 위해 level%2 == 0를 검사한다. 
		// 즉 검은색을 둘러싼 컨투어만 처리하기 위해서다
		// 그리고 컨투어를 구성하는 점의 수가 4개 이상 되어야 사각형 후보가 된다.
		if (level%2 == 0 && s->total >= 4)
		{
			// 바운딩 박스를 찾는 이유는 컨투어의 대략적인 크기를 알기 위해서다.
			// 크기에 따라 컨투어를 approximation 하는 정밀도를 조정한다.
			// 여기서는 대략 10%정도의 정밀도로 조정한다. (d*approx_param 부분)
			CvRect rect = cvBoundingRect (s);

			double d = sqrt ((double)rect.height*rect.width);

			const double d_th = 12.;
			const double approx_param = 0.1;

			// 컨투어의 대략적인 크기가 d_th보다 커야 한다.
			if (d > d_th) {
				CvSeq *ss = cvApproxPoly (s, s->header_size, storage, CV_POLY_APPROX_DP, d*approx_param, 0);
				// 컨투어를 approximation 하고나서 코너의 수가 4개(사각형)인지 검사한다.
				if (ss->total == 4) {
					// 추가적으로, 이 사각형의 두 대각선 교점이 사각형 중앙에 오는지 검사한다.
					if (checkRectCenter(ss))
					{
						// 마커를 찾았다. 마커 목록에 저장한다.
						sMarkerInfo mi;

						mi.width = MARKER_WIDTH;		// 실제 마커의 가로 길이 (단위: m)
						mi.height = MARKER_HEIGHT;		// 실제 마커의 세로 길이 (단위: m)
						mi.ID = -1;						// -1로 초기화
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
	bool invalid = false;	// 올바른 마커가 아니면 true
	for (unsigned int i=0; i<markers.size(); ++i)
	{
		invalid = false;

		// 검출된 마커의 코너로부터 서브픽셀 정확도로 코너 좌표를 다시 구한다.
		cvFindCornerSubPix (src, markers[i].corner, 4, cvSize(2, 2), cvSize(-1, -1), 
			cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.01));

		// src에서 찾은 마커의 영역으로부터 마커 영역만 추출한다.
		const int marker_size = 60;
		IplImage *img_marker = cvCreateImage (cvSize (marker_size, marker_size), IPL_DEPTH_8U, 1);
		extractMarkerImage (src, img_marker, markers[i]);

		// 마커는 6 x 6의 행렬로 구성된다.
		double code_matrix[6][6] = {0, };

		// 마커 내부의 픽셀들의 합으로부터 코드 값을 추출한다.
		extractCodeFromImage (img_marker, code_matrix);

		if (checkParity (code_matrix)) {
			int rotate_index = getRotation (code_matrix);
			if (0 <= rotate_index) {
				// 마커 인식 성공!!!

				// 마커의 코드를 포함한 행렬의 회전된 각도를 보정해 준다.
				rotateMatrix (code_matrix, rotate_index);
				rotateCorner (markers[i].corner, rotate_index);

				markers[i].ID = calcMarkerID (code_matrix);
				// TRACE ("Marker ID = %d\n", markers[i].ID);
				
				findMarkerPos3d (&markers[i]);

				drawMarkerAxis (&markers[i], dst);

				// 원본 마커 코드
				// cvNamedWindow ("Marker Image Org", CV_WINDOW_AUTOSIZE);
				// cvShowImage ("Marker Image Org", img_marker);

				// showMarkerCode (cvGetSize(img_marker), code_matrix);
			}else
				invalid = true;
		}else
			invalid = true;

		if (invalid)
		{	// 올바른 마커가 아님
			// 삭제
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

	const float ignoring_margin = 0.f;	// 원본 이미지로부터 마커 이미지로 복사하면서 무시할 테두리의 영역

	// 추출한 마커를 저장할 이미지 상의 좌표
	CvPoint2D32f dest_corner[4] =
	{ 
		{ -ignoring_margin,				-ignoring_margin}, 
		{ -ignoring_margin,				dst->height + ignoring_margin}, 
		{ dst->width + ignoring_margin,	dst->height + ignoring_margin}, 
		{ dst->width + ignoring_margin,	-ignoring_margin} 
	};

	CvMat *transform_matrix = cvCreateMat(3, 3, CV_32FC1);

	// 소스 이미지에서 마커의 코너에 대한 점들을 마커 이미지 상의 점들로 매핑하기 위한 변환 행렬을 구한다.
	cvGetPerspectiveTransform (mi.corner, dest_corner, transform_matrix);

	// 소스 이미지 상의 마커를 마커 이미지로 복사한다.
	cvWarpPerspective (src, dst, transform_matrix);

	cvReleaseMat (&transform_matrix);
}

void MarkerRecognizer::extractCodeFromImage (IplImage *src, double code_matrix[6][6])
{
	#define PIXEL_YX(img,y,x)	(unsigned char &)img->imageData[(y)*img->widthStep + (x)]

	assert (src->width == 60 && src->height == 60);

	// 마커 이미지를 6x6 격자로 쪼갠 후 각각의 격자 내부 픽셀들을 모두 더한다.
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
	
	// 더한 값을 0 ~ 1 사이의 값으로 정규화 하면서 최대값과 최소값을 찾는다.
	// 하나의 격자에는 100개의 픽셀이 더해지고 한 픽셀의 최대 값은 255이기 때문에
	// 더한 값을 100*255로 나누어주면 된다.
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			code_matrix[y][x] /= 100.*255;

			if (min_v > code_matrix[y][x]) min_v = code_matrix[y][x];
			if (max_v < code_matrix[y][x]) max_v = code_matrix[y][x];
		}
	}

	// 최대값과 최소값의 중간값을 찾는다.
	double mid_v = (min_v + max_v)/2.;

	// 중간값을 기준으로 검정색에 가까우면 1.을 흰색에 가까우면 0.을 대입한다.
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			code_matrix[y][x] = (code_matrix[y][x] < mid_v) ? 1. : 0.;
		}
	}
}

bool MarkerRecognizer::checkParity (double code_matrix[6][6])
{
	int sum = 0;

	// 테두리가 모두 제대로 있는지 검사한다.
	// 즉, 한 방향의 블럭 수는 6개이고 모서리가 4개이니까 
	// 합이 24개가 되어야 한다.
	for (int i = 0; i < 6; i++) {
		sum += (int)code_matrix[0][i];
		sum += (int)code_matrix[5][i];
		sum += (int)code_matrix[i][0];
		sum += (int)code_matrix[i][5];
	}
	if (sum != 24) return false;

	sum = 0;

	// 체크섬을 검사한다.
	// 테두리를 제외한 내부 블럭의 수는 1개가 되어야 한다.
	for (int y = 1; y < 5; y++) {
		for (int x = 1; x < 5; x++) {
			sum += (int)code_matrix[y][x];
		}
	}
	return (sum == 1);
}

int MarkerRecognizer::getRotation (double code_matrix[6][6])
{
	if      (code_matrix[1][1] && !code_matrix[1][4] && !code_matrix[4][4] && !code_matrix[4][1]) return 0;	// 정상
	else if (!code_matrix[1][1] && code_matrix[1][4] && !code_matrix[4][4] && !code_matrix[4][1]) return 1;	// 시계방향으로 90도 회전됨
	else if (!code_matrix[1][1] && !code_matrix[1][4] && code_matrix[4][4] && !code_matrix[4][1]) return 2; // 시계방향으로 180도 회전됨
	else if (!code_matrix[1][1] && !code_matrix[1][4] && !code_matrix[4][4] && code_matrix[4][1]) return 3; // 시계방향으로 270도 회전됨
	else return -1; // 있을수 없는 조합이다. 실패
}

void MarkerRecognizer::rotateMatrix (double code_matrix[6][6], int angle_idx)
{
	if (angle_idx == 0) return;

	double cb[6][6];

	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			switch (angle_idx) {
			case 1: cb[y][x] = code_matrix[x][5-y];		break; // 반시계 방향으로 90도 회전
			case 2: cb[y][x] = code_matrix[5-y][5-x];	break; // 반시계 방향으로 180도 회전
			case 3: cb[y][x] = code_matrix[5-x][y];		break; // 반시계 방향으로 270도 회전
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

	// 회전(rotation)과 이동(translation)을 계산하여 저장할 매트릭스 생성
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
	
	// 3차원 공간에서 마커의 위치와 방위를 찾는다.
	cvFindExtrinsicCameraParams2 (&object_points, &image_points, 
		calib._intrinsic_matrix, calib._distortion_coeffs, 
		&rotation, &translation);
}

void MarkerRecognizer::showMarkerCode (CvSize &size, double code_matrix[6][6])
{
	// 코드 블록으로부터 만들어낸 마커 코드를 이미지로 변환하여 표시한다.

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

	// 3차원 공간에서 마커의 x, y, z 좌표를 설정한다.
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

	// 마커의 x, y, z 좌표를 이미지로 프로젝션 한다.
	cvProjectPoints2 (&object_points, &rotation, &translation, calib._intrinsic_matrix, calib._distortion_coeffs, &image_points);

	// 2차원으로 프로젝션된 좌표를 그린다.
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[1][0]), cvRound(image_xy[1][1])), CV_RGB(255,0,0), 2);
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[2][0]), cvRound(image_xy[2][1])), CV_RGB(0,255,0), 2);
	cvLine (dst, cvPoint(cvRound(image_xy[0][0]), cvRound(image_xy[0][1])), cvPoint(cvRound(image_xy[3][0]), cvRound(image_xy[3][1])), CV_RGB(0,0,255), 2);

	// 마커의 ID를 표시한다.
	char buff[256];
	sprintf (buff, "     ID: %d", marker->ID);
	cvPutText(dst, buff, cvPointFrom32f(marker->corner[0]), &font, CV_RGB(255, 0, 0));
}