#include "Texture.h"
#include <fstream>
#include <string>
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace std;
using namespace Gdiplus;

void Texture::ReadJpg(std::string path)
{
    GdiplusStartupInput gdiplusstartupinput;
    ULONG_PTR gdiplustoken;
    GdiplusStartup(&gdiplustoken, &gdiplusstartupinput, NULL);

    string p = "Texture\\" + path;

    wstring infilename(p.begin(),p.end());

    Bitmap* bmp = new Bitmap(infilename.c_str());
    height = bmp->GetHeight();
    width = bmp->GetWidth();

    Color color;

    for (UINT y = 0; y < height; y++)
    {
        vector<Vector4> temp;
        for (UINT x = 0; x < width; x++)
        {
            bmp->GetPixel(x, y, &color);

            Vector4 _color(color.GetRed(), color.GetGreen(), color.GetBlue(), .0f);

            temp.push_back(_color);
        }
        tex.push_back(temp);
    }
     


    delete bmp;
    GdiplusShutdown(gdiplustoken);
}


/// <summary>
/// 获取纹理颜色
/// </summary>
/// <param name="u">纹理大小 * U 坐标的值</param>
/// <param name="v">纹理大小 * V 坐标的值</param>
/// <returns></returns>
Vector4 Texture::GetColor(float u, float v)
{
    int _u = u * width;
    int _v = v * height;
    if (_u < 0 || _u > width || _v < 0 || v > height || _u >= tex.size() || _v >= tex[_u].size())
    {
        return Vector4(0,0,0,0);
    }

    return tex[_u][_v];
}