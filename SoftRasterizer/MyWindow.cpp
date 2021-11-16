
#include "MyWindow.h"
#include <windows.h>

#pragma region 废弃

//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
//{
//	MyWindow window(hInstance);
//
//	window.Initialize(800,800);
//
//
//	return window.Run();
//}



//
// <summary>
//  主函数，主要用于创建窗口，基本不用更改
// </summary>
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
//{
//    int w = 800;
//    int h = 800;
//
//
//    WNDCLASSEX wcex = { 0 };
//    wcex.cbSize = sizeof(WNDCLASSEX);
//    wcex.style = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc = WndProc;
//    wcex.hInstance = hInstance;
//    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
//    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wcex.lpszClassName = L"simpleSoftRender";
//    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE((WORD)IDI_APPLICATION));
//    if (!RegisterClassEx(&wcex))
//    {
//        MessageBox(NULL,
//            L"Call to RegisterClassEx failed!",
//            L"Win32 Guided Tour",
//            NULL);
//
//        return 1;
//    }
//
//    HWND hWnd = CreateWindow(
//        L"simpleSoftRender",
//        L"simpleSoftRender",
//        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
//        CW_USEDEFAULT, CW_USEDEFAULT,
//        w, h,
//        NULL,
//        NULL,
//        hInstance,
//        NULL
//    );
//    if (!hWnd)
//    {
//        MessageBox(NULL,
//            L"Call to CreateWindow failed!",
//            L"Win32 Guided Tour",
//            NULL);
//
//        return 1;
//    }
//
//
//    ShowWindow(hWnd, nShowCmd);
//
//
//
//    UpdateWindow(hWnd);
//
//
//    MSG msg = {0};
//    while (msg.message != WM_QUIT)
//    {
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//   
//    return (int)msg.wParam;
//}
//
//

//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{   
//    return MyWindow::GetWindow()->MsgProc(hWnd,message,wParam,lParam);
//}


#pragma endregion


LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return MyWindow::GetWindow()->MsgProc(hwnd, msg, wParam, lParam);
}



MyWindow* MyWindow::mWindow = nullptr;
MyWindow* MyWindow::GetWindow()
{
    return mWindow;
}



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

    MyWindow theApp(hInstance);

    if (theApp.Initialize(800, 800) == false)
        return 0;

    return theApp.Run();
	
}


/// <summary>
///  初始化窗口
/// </summary>
bool MyWindow::Initialize(int w, int h)
{
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = MainWndProc;
    wcex.hInstance = mhAppInst;
    wcex.hIcon = LoadIcon(mhAppInst, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"simpleSoftRender";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE((WORD)IDI_APPLICATION));
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            L"Call to RegisterClassEx failed!",
            L"Win32 Guided Tour",
            NULL);

        return 1;
    }

    // 1.2 create window
    mhMainWnd = CreateWindow( L"simpleSoftRender",L"simpleSoftRender",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,NULL,NULL,mhAppInst,NULL);

    if (!mhMainWnd)
    {
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Win32 Guided Tour",
            NULL);

        return 1;
    }

    ShowWindow(mhMainWnd, SW_SHOW);
    InitRenderer(w, h, mhMainWnd);

    UpdateWindow(mhMainWnd);

    return true;
}


/// <summary>
///  初始化渲染器
/// </summary>
void MyWindow::InitRenderer(int w, int h, HWND hWnd)
{


    int width = w;
    int height = h;
    unsigned int* frameBuffer;

    // 1. 创建一个屏幕缓冲
    // 1.1 创建一个与当前设备兼容的DC
    HDC hDC = GetDC(hWnd);
    currentHDC = CreateCompatibleDC(hDC);
    ReleaseDC(hWnd, hDC);
    // 1.2 创建该DC的bitmap缓冲  32位色
    BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
        (DWORD)w * h * 4, 0, 0, 0, 0 } };
    currentBitMap = CreateDIBSection(currentHDC, &bi, DIB_RGB_COLORS, (void**)&frameBuffer, 0, 0);
    // 1.3 选择该bitmap到dc中
    oldBitMap = (HBITMAP)SelectObject(currentHDC, currentBitMap);


    if (scene == nullptr)
    {
        scene = new Scene();
    }
    scene->InitScene(w, h, frameBuffer);
}


/// <summary>
///  窗口运行
/// </summary>
int MyWindow::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
        else
        {
            Update(mhMainWnd);
        }
	}
    ShutDown();
	return (int)msg.wParam;
}


/// <summary>
///  窗口内容的更新
/// </summary>
void MyWindow::Update(HWND hWnd)
{
    scene->UpdateFrame();

    HDC hDC = GetDC(hWnd);
    BitBlt(hDC, 0, 0, scene->width, scene->height, currentHDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hDC);
}


/// <summary>
///  关闭窗口
/// </summary>
void MyWindow::ShutDown()
{
    if (currentHDC)
    {
        if (oldBitMap)
        {
            SelectObject(currentHDC, oldBitMap);
            oldBitMap = nullptr;
        }
        DeleteDC(currentHDC);
        currentHDC = nullptr;
    }

    if (currentBitMap)
    {
        DeleteObject(currentBitMap);
        currentBitMap = nullptr;
    }
}


/// <summary>
///  消息处理
/// </summary>
LRESULT MyWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        OnMouseDown(wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        OnMouseUp(wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
        return 0;
    case WM_MOUSEMOVE:
        OnMouseMove(wParam, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
        return 0;
    //case WM_KEYDOWN:
    //    OnKeyBoard(wParam);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }

    return 0;
}




void MyWindow::OnMouseDown(WPARAM btnState, int x, int y)
{
    scene->mLastMousePos.x = x;
    scene->mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}


void MyWindow::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void MyWindow::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        scene->dirty = true;

        float dx = MathHelper::EulerToPi(0.25f * static_cast<float>(x - scene->mLastMousePos.x));
        float dy = MathHelper::EulerToPi(0.25f * static_cast<float>(y - scene->mLastMousePos.y));

        scene->mTheta -= dx;
        scene->mPhi -= dy;

        scene->mPhi = MathHelper::Clamp(scene->mPhi, 0.1f, MathHelper::Pi - 0.1f);

    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        scene->dirty = true;
        float dx = 0.05f * static_cast<float>(x - scene->mLastMousePos.x);
        float dy = 0.05f * static_cast<float>(y - scene->mLastMousePos.y);

        scene->mRadius += dx - dy;

        scene->mRadius = MathHelper::Clamp(scene->mRadius, 5.0f, 20.0f);
    }

    scene->mLastMousePos.x = x;
    scene->mLastMousePos.y = y;

}


void MyWindow::OnKeyBoard(WPARAM btnState)
{
    if (GetAsyncKeyState('W') & 0x8000)
        scene->mTheta += 1.0f;

    if (GetAsyncKeyState('S') & 0x8000)
        scene->mTheta -= 1.0f;

    if (GetAsyncKeyState('A') & 0x8000)
        scene->mPhi += 1.0f;

    if (GetAsyncKeyState('D') & 0x8000)
        scene->mPhi += 1.0f;
}