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


    bool openShadow = false;
    bool openLight = true;
    bool useTexture = true;



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


    void ClearFrameBuffer();
    void VertexShader();
    void FragmentShader();
   


    void DrawTriangleWithBoundingBox(std::vector<Vertex>& triangle, int& materialIndex, std::string& textureIndex);
    void DrawTriangleWithMidPoint(std::vector<Vertex>& triangle, int& materialIndex, std::string& textureIndex);

    void BuildShadowWithBoundingBox(std::vector<Vertex>& triangle);
    void BuildShadowWithMidPoint(std::vector<Vertex>& triangle);




    void DrawPointWithTriangleLerp(std::vector<Vertex>& triangle, Vector3& point,
        int& index, int& materialIndex, std::string& texName);
    void DrawPointBilinearLerp(Vertex& point, int& index,  int& materialIndex, std::string& texName);



    void DrawLeftTriangle(Vertex& v1, Vertex& v2, Vertex& v3, int& materialIndex, std::string& texName);
    void DrawRightTriangle(Vertex& v1, Vertex& v2, Vertex& v3, int& materialIndex, std::string& texName);
    void DrawLeftTriangle(Vertex& v1, Vertex& v2, Vertex& v3, bool isShadow);
    void DrawRightTriangle(Vertex& v1, Vertex& v2, Vertex& v3, bool isShadow);




    void ShadowVertexShader();
    void ShadowFragmentShader();




    bool CalcShadow(Vector4 pos);
    void CalcLight(Vertex& point, int& materialIndex);


    void LerpVertex(Vertex& target, Vertex& a, Vertex& b, float t, bool isShadow);
    void SortTriangle(std::vector<Vertex>& triangle,bool flag);
    bool IsInTriangle(Vector3& point, Vector4& v1, Vector4& v2, Vector4& v3);
    void ComputeBarycentric(std::vector<Vertex> triangle, float x, float y, 
        float& alpha, float& beat, float& gamma, bool isLight);
    bool JudgeBackOrFront(std::vector<Vertex> triangle);
    long GetIndex(int x, int y);
};
