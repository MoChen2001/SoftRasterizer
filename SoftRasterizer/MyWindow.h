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


    // ��Ҫ�ĳ�������
    Scene* scene = nullptr;

    int count = 0;

    HDC currentHDC = nullptr;
    HBITMAP currentBitMap = nullptr;
    HBITMAP oldBitMap = nullptr;


public:

    bool Initialize(int w, int h);
    int Run();

    //// ��ʼ����Ⱦ�� ��Ļ���� ��Ļ����
    void InitRenderer(int w, int h, HWND hWnd);
       
    // ÿ֡����
    void Update(HWND hWnd);
    // ֹͣ
    void ShutDown();
    // �����Ϣ����
    LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


    void OnKeyBoard(WPARAM btnState);
    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);


};





