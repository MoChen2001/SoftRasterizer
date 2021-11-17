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

            Vector4 _color(color.GetRed(), color.GetGreen(), color.GetBlue(), 0.0f);

            temp.push_back(_color);
        }
        tex.push_back(temp);
    }
     


    delete bmp;
    GdiplusShutdown(gdiplustoken);
}



Vector4 Texture::GetColor(int u, int v)
{
    if (u < 0 || u > height || v < 0 || v > width || u >= tex.size() || v >= tex[u].size())
    {
        return Vector4(0,0,0,0);
    }
    return tex[u][v];
}