#pragma once


#include <math.h>
#include <vector>
#include <memory>
#include <windows.h>
#include "Scene.h"

/// <summary>
///  主要是利用 win32 API 生成一个窗口同时对其中的每个像素进行颜色的存储处理以及深度的存储处理
/// </summary>
class MyWindow
{
public:
    
    // 主要的场景变量
    Scene* scene = nullptr;

    int count = 0;

    HDC currentHDC = nullptr;
    HBITMAP currentBitMap = nullptr;
    HBITMAP oldBitMap = nullptr;


public:

    // 初始化渲染器 屏幕长宽 屏幕缓冲
    void InitRenderer(int w, int h, HWND hWnd);
    // 每帧更新
    void Update(HWND hWnd);
    // 停止
    void ShutDown();
    // 消息处理
    void OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);


public:

    // 单例模式
    static MyWindow* GetInstance()
    {
        if (myWindow == NULL)
        {
            myWindow = new MyWindow();
        }
        return myWindow;
    }

private:
    static MyWindow* myWindow;

    MyWindow() { }
    ~MyWindow() { }
    MyWindow(const MyWindow&) = delete;
    MyWindow& operator=(const MyWindow&) = delete;


};


MyWindow* MyWindow::myWindow = NULL;


void MyWindow::InitRenderer(int w, int h, HWND hWnd)
{
    if (count == 0)
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

        // 清理屏幕缓冲
        scene->ClearFrameBuffer();
        count++;
    }


}

// 窗口的更新
void MyWindow::Update(HWND hWnd)
{
    scene->UpdateFrame();

    HDC hDC = GetDC(hWnd);
    BitBlt(hDC, 0, 0, scene->width, scene->height, currentHDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hDC);
}

void MyWindow::OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool isPressed = false;
    static LPARAM lpCur = 0;
    switch (message)
    {
    case WM_MOUSEWHEEL:
        
        break;
    case WM_LBUTTONDOWN:
        isPressed = true;
        lpCur = lParam;
        break;
    case WM_LBUTTONUP:
        isPressed = false;
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
        {
            short xMove = LOWORD(lParam) - LOWORD(lpCur);
            short yMove = HIWORD(lParam) - HIWORD(lpCur);
            lpCur = lParam;
            
        }
        break;  
    default:
        break;
    }
}


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