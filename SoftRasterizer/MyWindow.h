#pragma once


#include <math.h>
#include <vector>
#include <memory>
#include <windows.h>
#include "Scene.h"

/// <summary>
///  ��Ҫ������ win32 API ����һ������ͬʱ�����е�ÿ�����ؽ�����ɫ�Ĵ洢�����Լ���ȵĴ洢����
/// </summary>
class MyWindow
{
public:
    
    // ��Ҫ�ĳ�������
    Scene* scene = nullptr;

    int count = 0;

    HDC currentHDC = nullptr;
    HBITMAP currentBitMap = nullptr;
    HBITMAP oldBitMap = nullptr;


public:

    // ��ʼ����Ⱦ�� ��Ļ���� ��Ļ����
    void InitRenderer(int w, int h, HWND hWnd);
    // ÿ֡����
    void Update(HWND hWnd);
    // ֹͣ
    void ShutDown();
    // ��Ϣ����
    void OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);


public:

    // ����ģʽ
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

        // 1. ����һ����Ļ����
        // 1.1 ����һ���뵱ǰ�豸���ݵ�DC
        HDC hDC = GetDC(hWnd);
        currentHDC = CreateCompatibleDC(hDC);
        ReleaseDC(hWnd, hDC);
        // 1.2 ������DC��bitmap����  32λɫ
        BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
            (DWORD)w * h * 4, 0, 0, 0, 0 } };
        currentBitMap = CreateDIBSection(currentHDC, &bi, DIB_RGB_COLORS, (void**)&frameBuffer, 0, 0);
        // 1.3 ѡ���bitmap��dc��
        oldBitMap = (HBITMAP)SelectObject(currentHDC, currentBitMap);


        if (scene == nullptr)
        {
            scene = new Scene();
        }
        scene->InitScene(w, h, frameBuffer);

        // ������Ļ����
        scene->ClearFrameBuffer();
        count++;
    }


}

// ���ڵĸ���
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