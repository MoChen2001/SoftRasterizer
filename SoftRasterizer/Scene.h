#pragma once

#include <iostream>
#include <windows.h>
#include "VectorX.h"
#include "Camera.h"
#include "RenderItem.h"

/// <summary>
///  场景类，渲染的核心
/// </summary>
class Scene
{
public:
	int width;
	int height;

    // 摄像机的旋转使用球坐标
    float mTheta = 1.5f * MathHelper::Pi;  // xz 平面的角度,从 z 到 x
    float mPhi = MathHelper::PiDivTwo - 0.1f;    // 竖直平面的角度  [- pi/2, pi/2] 从 z 到 y
    float mRadius = 5.0f;

    POINT mLastMousePos;


    bool dirty = false;

    // 背景颜色
    Vector4 bg_color{0,0,0,0};
    Matrix4x4 viewProt;

    // 帧缓冲
	unsigned int* frameBuffer;
    // 深度缓冲
	std::shared_ptr<float[]> depthBuffer = nullptr;

    // 摄像机
    Camera camera;

    // 渲染项
    RenderItem mRenderItem;
    // 场景物品，保留原始信息
    RenderItem mSceneItem;

public:

    void InitScene(int w, int h, unsigned int*& frameBuffer);

    void BuildRendreItem();
    void LoadData();
    void TransformItem();
    void DrawItem();
    void ClearFrameBuffer();
    void UpdateFrame();
    void UpdateCamera();


    void OnKeyBoard(WPARAM wParam);

private:

    // 绘制三角形
    void DrawTriangle(std::vector<Vertex> triangle);

    // 判断点是不是在三角形内
    bool IsInTriangle(Vector3& point, Vector4& v1, Vector4& v2, Vector4& v3);

    /// <summary>
    ///  获得索引
    /// </summary>
    long GetIndex(int x, int y);

    
    void ComputeBarycentric2D(std::vector<Vertex> triangle, float x, float y, float& alpha, float& beat, float& gamma);

};
