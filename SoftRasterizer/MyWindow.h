#pragma once


#include <math.h>
#include <vector>
#include <memory>
#include <windows.h>
#include "Scene.h"
#include "MyTime.h"

/// <summary>
///  主要是利用 win32 API 生成一个窗口同时对其中的每个像素进行颜色的存储处理以及深度的存储处理
/// </summary>
class MyWindow
{
public:
    MyWindow(HINSTANCE hInstance) 
        : mhAppInst(hInstance) 
    { 
        mWindow = this; 
    };

    MyWindow(const MyWindow& rhs) = delete;
    MyWindow& operator=(const MyWindow& rhs) = delete;
    ~MyWindow() {};


public:

    static MyWindow* mWindow;
    static MyWindow* GetWindow();



    HINSTANCE mhAppInst = nullptr; // application instance handle
    HWND      mhMainWnd = nullptr; // main window handle


    // 主要的场景变量
    Scene* scene = nullptr;

    HDC currentHDC = nullptr;
    HBITMAP currentBitMap = nullptr;
    HBITMAP oldBitMap = nullptr;
    MyTime mTimer;

public:

    bool Initialize(int w, int h);
    int Run();

    //// 初始化渲染器 屏幕长宽 屏幕缓冲
    void InitRenderer(int w, int h, HWND hWnd);
       
    // 每帧更新
    void Update(HWND hWnd);
    // 停止
    void ShutDown();
    // 鼠标消息处理
    LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);


};





