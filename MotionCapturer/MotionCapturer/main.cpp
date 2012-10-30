// MotionCapturer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <string>

#include "source/animation/Animation.h"
#include "source/animation/KeyFrame.h"
#include "source/animation/BoneData.h"
#include "source/kinect/Kinect.h"
#include "source/kinect/KinectManager.h"
#include "source/marker/MarkerRecognizer.h"
#include "source/model/Bone.h"
#include "source/util/BmpExporter.h"
#include "source/util/Matrix.h"
#include "source/Engine.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hWnd;
HDC hDC;

/// TODO test
float angle = 0.f;
bool flag = true;
/// end test


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
bool EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

using namespace std;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	HACCEL hAccelTable;
    HGLRC hRC;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MOTIONCAPTURER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOTIONCAPTURER));


	/// 초기화
	// Open GL 초기화
    if (EnableOpenGL(hWnd, &hDC, &hRC) == false)
    	goto END;

	// 엔진 초기화

	// 리사이즈
	ENGINE.resize(480, 800);

	// 키넥트 연결
	int connectedCount = KINECT_MANAGER.connectKinects(10);

    // Main message loop
    MSG msg = {0};
    while (ENGINE.getRunningState())
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
			ENGINE.run();

			// 스왑
            SwapBuffers(hDC);
        }
    }
	

	/// 종료
END:
	// shutdown OpenGL
	DisableOpenGL(hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hWnd);

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOTIONCAPTURER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MOTIONCAPTURER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND &hWnd)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

bool initializeKinect()
{
	ENGINE.clearPointCloud();
	KINECT_MANAGER.deconnectKinects();

	/// 키넥트 센서로부터 정보를 받아와서 가공
	KINECT_MANAGER.connectKinects(1);
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		Kinect *kinect = KINECT_MANAGER.getKinect(i);

		/// 마커 인식
		if (kinect->recognizeMakerAccurately(10, 3) == false)
		{
			wstringstream wstream;
			wstream << i << L"번 키넥트의 마커를 인식할 수 없습니다.";
			MessageBox(hWnd, wstream.str().c_str(), L"에러", MB_OK);
			KINECT_MANAGER.deconnectKinects();
			return false;
		}
		
		KINECT_MANAGER.connectKinects(1);
	}

	return true;
}

// 배경에 포인트 클라우드 추가
void addPointCloudToBackground(CloudElement cloud[], int size)
{
	const float pointCloudUnit = ENGINE.getPointCloudUnit();
	NormalizedPoint np;
	for (int i=0; i<size; ++i)
	{
		ENGINE.normalizePoint(cloud[i].position, np);
		ENGINE.addBackground(np);
	}
}

// 포인트 클라우드 갱신
void refreshPointCloud()
{
	ENGINE.clearPointCloud();

	/// 키넥트 센서로부터 정보를 받아와서 가공
	for (int i=0; i<KINECT_MANAGER.getKinectCount(); ++i)
	{
		Kinect *kinect = KINECT_MANAGER.getKinect(i);

				
		/// depth와 color 버퍼를 갱신하고 매핑한다.
		bool depthIsRefreshed, colorIsRefreshed;
		do
			depthIsRefreshed = kinect->refreshDepthBuffer() >= 0;
		while(depthIsRefreshed == false);
		do
			colorIsRefreshed = kinect->refreshColorBuffer() >= 0;
		while (colorIsRefreshed == false);

		kinect->mapColorToDepth();
		

		/// 포인트 클라우드 변환, 추가
		int cloudSize = 640*480;
		CloudElement *cloud = new CloudElement[cloudSize];

		// 변환
		kinect->transformPointCloud(cloud);

		// 포인트 클라우드 큐에 넣어놓기
		ENGINE.addPointCloud(cloud, cloudSize);
		
	}

}

// 스켈레톤 갱신
void refreshSkeleton()
{
	if (KINECT_MANAGER.getKinectCount() <= 0)
		return;

	Kinect *kinect = KINECT_MANAGER.getKinect(0);
	kinect->refreshSkeleton();
	kinect->transformSkeleton();
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			ENGINE.setRunningState(false);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		ENGINE.setRunningState(false);
		break;

	case WM_SIZE:
		ENGINE.resize(LOWORD(lParam), HIWORD(lParam));
		break;
		
    case WM_KEYDOWN:
	{
		int nKey = static_cast<int>(wParam);

		if (nKey == 'A')
		{
			ENGINE.getCamera().yaw(deg2rad(2));
		}
		if (nKey == 'D')
		{
			ENGINE.getCamera().yaw(-deg2rad(2));
		}
		
		if (nKey == 'W')
		{
			ENGINE.getCamera().pitch(deg2rad(2));
		}
		if (nKey == 'S')
		{
			ENGINE.getCamera().pitch(-deg2rad(2));
		}

		if (nKey == VK_ESCAPE)
		{
			// 포인트 클라우드 큐 비우기
			ENGINE.clearPointCloud();
		}

		if (nKey == VK_RETURN)
		{
			ENGINE.rotationFlag = false;
			ENGINE.angle = 0.f;
		}

		if (nKey == VK_SPACE)
		{
			ENGINE.rotationFlag = true;
		}
		
		break;
	}

	case WM_MOUSEWHEEL:
	{
		float wheelValue = (float)(short)HIWORD(wParam);
		ENGINE.getCamera().moveForward(wheelValue * 0.001f);

		break;
	}

	case WM_LBUTTONDOWN:
		if (initializeKinect() == false)
		{
			MessageBox(hWnd, L"실패", L"dd", MB_OK);
		}else
			MessageBox(hWnd, L"성공", L"dd", MB_OK);
		break;

	case WM_MBUTTONDOWN:
		// 배경 촬영

		break;

	case WM_RBUTTONDOWN:
		// 포인트 클라우드 갱신
		refreshPointCloud();
		break;
		// 스켈레톤 갱신
		refreshSkeleton();

		// 키프레임 추가
		if (KINECT_MANAGER.getKinectCount() > 0)
		{
			KeyFrame *key = ENGINE.getAnimation()->addKeyFrame();
			key->setDuration(100);
			key->setBoneDataFromKinect(KINECT_MANAGER.getKinect(0));

			ENGINE.getAnimation()->refresh();
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


bool EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);


    /// glew
    GLenum err = glewInit();
    if (err != GLEW_OK)
    	return false;

    if (!WGLEW_ARB_pixel_format)
    {	// 옛날 그래픽 카드라서 지원안하는 경우
    	// 그냥 예전 버전의 RC 사용
    	return true;
    }

    const int attribList[] =
    {
    	WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    	WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    	WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    	WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    	WGL_COLOR_BITS_ARB, 32,
    	WGL_DEPTH_BITS_ARB, 16,
    	WGL_STENCIL_BITS_ARB, 8,
    	0		// end
    };
    uint numFormats;
    wglChoosePixelFormatARB(*hDC, attribList, null, 1, &iFormat, &numFormats);
    SetPixelFormat(*hDC, iFormat, null);

	wglMakeCurrent(*hDC, *hRC);

    return true;
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
