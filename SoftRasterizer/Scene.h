#pragma once

#include <iostream>
#include <windows.h>
#include <unordered_map>
#include "Geometry.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"

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
    float mPhi = MathHelper::PiDivTwo;    // 竖直平面的角度  [- pi/2, pi/2] 从 z 到 y
    float mRadius = 15.0f;

    POINT mLastMousePos;
    float minDis = 8.0f;
    float maxDis = 30.0f;
    bool cameraMove = false;


    bool openShadow = false;
    bool openLight = true;
    bool useTexture = true;



    // 背景颜色
    Vector4 bg_color{150.0f,150.0f,150.0f,0};
    // 视口变换矩阵
    Matrix4x4 viewProt;
    // 帧缓冲
	unsigned int* frameBuffer;
    // 深度缓冲
	std::shared_ptr<float[]> depthBuffer = nullptr;
    // 阴影贴图,和深度缓冲区一样大
    std::shared_ptr<float[]> shadowMap = nullptr;
    // 主光源的视图矩阵，默认第一个光照为主光源
    Matrix4x4 lightView;
    // 主光源的投影矩阵，默认第一个光照为主光源
    Matrix4x4 lightProj;



    // 摄像机
    Camera camera;
    // 渲染项
    std::unordered_map<std::string, std::unique_ptr<Geometry>> mGeos;
    // 所有生成的材质
    std::vector<Material> mMaterials;
    // 所有的贴图资源
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    // 所有生成的光源
    std::vector<Light> mLights;
    // 环境光
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
