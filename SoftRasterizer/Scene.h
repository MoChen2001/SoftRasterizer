#pragma once

#include <iostream>
#include <windows.h>
#include <unordered_map>
#include "Geometry.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"

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
    float mPhi = MathHelper::PiDivTwo;    // ��ֱƽ��ĽǶ�  [- pi/2, pi/2] �� z �� y
    float mRadius = 15.0f;

    POINT mLastMousePos;
    float minDis = 8.0f;
    float maxDis = 30.0f;
    bool cameraMove = false;



    // ������ɫ
    Vector4 bg_color{150.0f,150.0f,150.0f,0};
    // �ӿڱ任����
    Matrix4x4 viewProt;
    // ֡����
	unsigned int* frameBuffer;
    // ��Ȼ���
	std::shared_ptr<float[]> depthBuffer = nullptr;
    // ��Ӱ��ͼ,����Ȼ�����һ����
    std::shared_ptr<float[]> shadowMap = nullptr;
    // ����Դ����ͼ����Ĭ�ϵ�һ������Ϊ����Դ
    Matrix4x4 lightView;
    // ����Դ��ͶӰ����Ĭ�ϵ�һ������Ϊ����Դ
    Matrix4x4 lightProj;



    // �����
    Camera camera;
    // ��Ⱦ��
    std::unordered_map<std::string, std::unique_ptr<Geometry>> mGeos;
    // �������ɵĲ���
    std::vector<Material> mMaterials;
    // ���е���ͼ��Դ
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    // �������ɵĹ�Դ
    std::vector<Light> mLights;
    // ������
    Vector4 ambient_Color{ 10, 10, 10 ,10};


    


public:

    void InitScene(int w, int h, unsigned int*& frameBuffer);
    void UpdateSceneData();
    void DrawScene();


    void BuildMaterial();
    void BuildRendreItem();
    void BuildLight();
    void BuildTexture();
    void BuildShadowMap();

    void UpdateCamera();

    void ShadowShader();
    void ShadowFragmentShadow(std::vector<Vertex>& triangle);


    void VertexShader();
    void FragmentShaderWithBoundingBox(std::vector<Vertex>& triangle, int& materialIndex, std::string& textureIndex);
    void FragmentShaderWithTriangle(std::vector<Vertex>& triangle, int& materialIndex, std::string& textureIndex);
    void DrawItem();
    void ClearFrameBuffer();
   

    // ����һ����
    void DrawPoint(std::vector<Vertex>& triangle, Vector3& pointint,
        int& index, int& materialIndex, std::string& texName);

    // �����Ƿ�����Ӱ
    bool CalcShadow(Vector4 pos);

    // �������
    void CalcLight(Vertex& point, int& materialIndex);

    // �жϵ��ǲ�������������
    bool IsInTriangle(Vector3& point, Vector4& v1, Vector4& v2, Vector4& v3);

    /// <summary>
    ///  �������
    /// </summary>
    long GetIndex(int x, int y);

    /// <summary>
    ///  ������������
    /// </summary>
    void ComputeBarycentric(std::vector<Vertex> triangle, float x, float y, 
        float& alpha, float& beat, float& gamma, bool isLight);

    /// <summary>
    ///  �ж������滹�Ǳ���
    /// </summary>
    bool JudgeBackOrFront(std::vector<Vertex> triangle);

};
