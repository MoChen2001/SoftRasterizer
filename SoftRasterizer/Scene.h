#pragma once

#include <iostream>
#include <windows.h>
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

    // ���������תʹ��������
    float mTheta = 1.5f * MathHelper::Pi;  // xz ƽ��ĽǶ�,�� z �� x
    float mPhi = MathHelper::PiDivTwo - 0.1f;    // ��ֱƽ��ĽǶ�  [- pi/2, pi/2] �� z �� y
    float mRadius = 5.0f;

    POINT mLastMousePos;


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
    // ������Ʒ������ԭʼ��Ϣ
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
