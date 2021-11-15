
#include "MyWindow.h"
#include <windows.h>




/// <summary>
///  主函数，主要用于创建窗口，基本不用更改
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    int w = 800;
    int h = 800;


    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
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
    HWND hWnd = CreateWindow(
        L"simpleSoftRender",
        L"simpleSoftRender",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        w, h,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (!hWnd)
    {
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Win32 Guided Tour",
            NULL);

        return 1;
    }


    ShowWindow(hWnd, nShowCmd);
    MyWindow::GetInstance()->InitRenderer(w, h, hWnd);


    UpdateWindow(hWnd);
    MyWindow::GetInstance()->Update(hWnd);


    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
           // MyWindow::GetInstance()->Update(hWnd);
        }
    }

    MyWindow::GetInstance()->ShutDown();
    return (int)msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEWHEEL:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_KEYDOWN:
        MyWindow::GetInstance()->OnMouseMessage(message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}