#pragma once

#include <iostream>
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

public:

    void InitScene(int w, int h, unsigned int*& frameBuffer);

    void BuildRendreItem();
    void BuildCamera();
    void TransformItem();
    void DrawItem();
    void ClearFrameBuffer();
    void UpdateFrame();


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
