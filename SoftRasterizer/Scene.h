#pragma once

#include <iostream>
#include "VectorX.h"
#include "Camera.h"
#include "RenderItem.h"

/// <summary>
///  �����࣬��Ⱦ�ĺ���
/// </summary>
class Scene
{
public:
	int width;
	int height;


    bool dirty = false;

    // ������ɫ
    Vector4 bg_color{0,0,0,0};
    Matrix4x4 viewProt;

    // ֡����
	unsigned int* frameBuffer;
    // ��Ȼ���
	std::shared_ptr<float[]> depthBuffer = nullptr;

    // �����
    Camera camera;

    // ��Ⱦ��
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

    // ����������
    void DrawTriangle(std::vector<Vertex> triangle);

    // �жϵ��ǲ�������������
    bool IsInTriangle(Vector3& point, Vector4& v1, Vector4& v2, Vector4& v3);

    /// <summary>
    ///  �������
    /// </summary>
    long GetIndex(int x, int y);

    
    void ComputeBarycentric2D(std::vector<Vertex> triangle, float x, float y, float& alpha, float& beat, float& gamma);

};
