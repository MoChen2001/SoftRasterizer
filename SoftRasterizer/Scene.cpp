#include "Scene.h"


/// <summary>
/// 初始化场景，初始化长度高度以及帧缓冲和深度缓冲
/// </summary>
void Scene::InitScene(int w, int h, unsigned int*& frameBuffer)
{
    width = w;
    height = h;


    viewProt = { (float)width / 2, 0, 0,  (float)width / 2,
    0, (float)height / 2, 0, (float)height / 2,
    // 为了便于计算
    0, 0, 1.0f, 0.0f,
    0, 0, 0, 1.0f };

    camera.SetLens(0.25f * MathHelper::Pi, 1.0f, 0.1f, 40.0f);


    this->frameBuffer = frameBuffer;
    depthBuffer.reset(new float[w * h]);
    shadowMap.reset(new float[w * h]);

    BuildMaterial();
    BuildTexture();
    BuildRendreItem();
    BuildLight();
    BuildShadowMap();


}

/// <summary>
///  更新帧
/// </summary>
void Scene::UpdateSceneData()
{
    UpdateCamera();

}

/// <summary>
///  绘制场景
/// </summary>
void Scene::DrawScene()
{
    if (cameraMove)
    {
        ClearFrameBuffer();
        VertexShader();
        FragmentShader();
        cameraMove = false;
    }
}



/// <summary>
///  创建材质
/// </summary>
void Scene::BuildMaterial()
{
    Material brick;
    brick.name = "brick";
    brick.diffuse = Vector4(0.5f, 0.5f, 0.5f,0.0f);
    brick.fresnel = Vector4(0.05f, 0.05f, 0.05f, 0.0f);
    brick.gloss = 0.3f;

    Material tile;
    tile.name = "tile";
    tile.diffuse = Vector4(0.3f, 0.3f, 0.3f, 0.0f);
    tile.fresnel = Vector4(0.2f, 0.2f, 0.2f,0.0f);
    tile.gloss = 0.1f;

    mMaterials.push_back(std::move(brick));
    mMaterials.push_back(std::move(brick));

}

/// <summary>
///  创建光源
/// </summary>
void Scene::BuildLight()
{
    Light l1;
    l1.dir = Vector3(1,1,1).Normalize();
    l1.intensity = Vector4(5.0f,5.0f,5.0f,0.5f);

    Light l2;
    l2.dir = Vector3(1,-0.7,-0.3);
    l2.intensity = Vector4(1.0f, 1.0f, 1.0f, 0.5f);

    mLights.push_back(std::move(l1));
    mLights.push_back(std::move(l2));

    Vector3 lightPos = l1.dir * -6;

    Vector3 lightTarget = l1.dir * -1;
    Vector3 lightUp(0, 1, 0);
    Vector3 lightRight = lightTarget.Cross(lightUp);
    lightUp = lightRight.Cross(lightTarget);

    // 默认第一个光照为主光源
    // 光源的位置取场景包围球的所在位置，
    // 场景包围球半径应该为 3 
    lightView = { lightRight.x, lightRight.y, lightRight.z, lightPos.Dot(lightRight) * -1,
            lightUp.x, lightUp.y, lightUp.z,  lightPos.Dot(lightUp) * -1,
            lightTarget.x, lightTarget.y, lightTarget.z , lightPos.Dot(lightUp) * -1,
            0, 0, 0, 1 };

    Vector4 center(0, 0, 0, 0);

    center = lightView * center;

    float n = center.z - 3.0f;
    float f = center.z + 3.0f;

    /// <summary>
    ///  平行光的正交投影，应该是场景包围盒的缩放到 齐次空间
    /// </summary>
    lightProj = { 1.0f / 3.0f , 0, 0, 0,
        0,  1.0f / 3.0f, 0, 0,
        0, 0,  1.0f / 6.0f,   -n / 6.0f,
        0, 0, 0, 1 };

}

/// <summary>
///  构建纹理
/// </summary>
void Scene::BuildTexture()
{
    auto tex1 = std::make_unique<Texture>();
    tex1->ReadJpg("crate_1.jpg");

    auto tex2 = std::make_unique<Texture>();
    tex2->ReadJpg("hmap.jpg");

    mTextures["Crate"] = std::move(tex1);
    mTextures["Hmap"] = std::move(tex2);
}

/// <summary>
///  初始化渲染项
/// </summary>
void Scene::BuildRendreItem()
{
    Cube mCube;
    auto cube = std::make_unique<Geometry>();

    for (int i = 0; i < mCube.vertexs.size(); i++)
    {
        cube->vertexs.push_back(mCube.vertexs[i]);
    }
    for (int i = 0; i < mCube.indexs.size(); i++)
    {
        cube->indexs.push_back(mCube.indexs[i]);
    }
    cube->worldMatrix = mCube.worldMatrix;
    cube->materialIndex = 0;
    cube->textureIndex = "Crate";


    Plane mPlane;
    auto plane = std::make_unique<Geometry>();

    for (int i = 0; i < mPlane.vertexs.size(); i++)
    {
        plane->vertexs.push_back(mPlane.vertexs[i]);
    }
    for (int i = 0; i < mPlane.indexs.size(); i++)
    {
        plane->indexs.push_back(mPlane.indexs[i]);
    }
    plane->worldMatrix = mPlane.worldMatrix;
    plane->materialIndex = 1;
    plane->textureIndex = "Crate";

    mGeos["cube"] = std::move(cube);
    mGeos["plane"] = std::move(plane);
}

/// <summary>
///  创建阴影贴图
/// </summary>
void Scene::BuildShadowMap()
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            shadowMap[GetIndex(col, row)] = 0.5f;
        }
    }
    ShadowVertexShader();
    ShadowFragmentShader();
}





/// <summary>
///  更新摄像机
/// </summary>
void Scene::UpdateCamera()
{
    Vector3 mEyePos;

    // 球坐标
    mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
    mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
    mEyePos.y = mRadius * cosf(mPhi);

    camera.SetPosition(mEyePos);

    Vector3 target(0, 0, 0);

    target = target - mEyePos;
    target = target.Normalize();

    camera.SetTarget(target);

    cameraMove = true;
}






/// <summary>
/// 清理帧缓冲
/// </summary>
void Scene::ClearFrameBuffer()
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int idx = row * width + col;
            // 默认背景色浅蓝 R123 G195 B221
            frameBuffer[GetIndex(col, row)] = bg_color.GetIntColor();
            // 深度缓冲区 1.0f
            depthBuffer[GetIndex(col, row)] = 1.0f;
        }
    }

}

/// <summary>
///  主要进行顶点着色器的事情，不过多了一个视口变换以及透视除法
///  这是为了方便之后绘制三角形使用
/// </summary>
void Scene::VertexShader()
{
    for (auto i = mGeos.begin(); i != mGeos.end(); i++)
    {
        for (int j = 0; j < i->second->vertexs.size(); j++)
        {
            Vector4 pos = i->second->vertexs[j].modelPos;

            i->second->vertexs[j].worldPos = i->second->worldMatrix * pos;

            pos = i->second->vertexs[j].worldPos;
            pos = camera.GetMyView() * pos;

            pos = camera.GetMyProj() * pos;

            float w = 1 / pos.w;
            pos = pos * w;

            pos = viewProt * pos;

            i->second->vertexs[j].screenPos = pos;
        }
    }
}

/// <summary>
///  片元着色器
/// </summary>
void Scene::FragmentShader()
{

    for (auto i = mGeos.begin(); i != mGeos.end(); i++)
    {
        int n = i->second->indexs.size();
        for (int j = 0; j < n; j += 3)
        {
            std::vector<Vertex> vertex_triangle;
            if (j + 1 > n || j + 2 > n)
            {
                break;
            }

            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 1]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 2]]);

            // 背面剔除
            if (JudgeBackOrFront(vertex_triangle))
            {
                // 包围盒绘制
                //DrawTriangleWithBoundingBox(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
                
                // 直接绘制
                DrawTriangleWithMidPoint(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
            }



        }
    }
}




/// <summary>
///  阴影贴图的着色器，
/// 等于重新渲染一遍场景，不过是在光源的视角下
/// </summary>
void Scene::ShadowVertexShader()
{
    for (auto i = mGeos.begin(); i != mGeos.end(); i++)
    {
        for (int j = 0; j < i->second->vertexs.size(); j++)
        {
            Vector4 pos = i->second->vertexs[j].modelPos;

            pos = i->second->worldMatrix * pos;



            pos = lightView * pos;

            pos = lightProj * pos;

            pos = viewProt * pos;

            i->second->vertexs[j].lightScreenPos = pos;
        }
    }

}

/// <summary>
///  生成阴影贴图
/// </summary>
void Scene::ShadowFragmentShader()
{

    for (auto i = mGeos.begin(); i != mGeos.end(); i++)
    {
        int n = i->second->indexs.size();
        for (int j = 0; j < n; j += 3)
        {
            std::vector<Vertex> vertex_triangle;
            if (j + 1 > n || j + 2 > n)
            {
                break;
            }
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 1]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 2]]);


            //BuildShadowWithBoundingBox(vertex_triangle);
            BuildShadowWithMidPoint(vertex_triangle);
        }
    }


}





/// <summary>
/// 绘制三角形
/// 直接使用三角形的包围盒进行判断绘制，消耗会比较大，不推荐使用
/// </summary>
void Scene::DrawTriangleWithBoundingBox(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
{

    // 包围盒
    int minX = (int)MathHelper::Min(MathHelper::Min(triangle[0].screenPos.x, triangle[1].screenPos.x), triangle[2].screenPos.x);
    int minY = (int)MathHelper::Min(MathHelper::Min(triangle[0].screenPos.y, triangle[1].screenPos.y), triangle[2].screenPos.y);

    int maxX = (int)MathHelper::Max(MathHelper::Max(triangle[0].screenPos.x, triangle[1].screenPos.x), triangle[2].screenPos.x);
    int maxY = (int)MathHelper::Max(MathHelper::Max(triangle[0].screenPos.y, triangle[1].screenPos.y), triangle[2].screenPos.y);


    // 遍历包围盒并绘制
    for (int i = minY; i <= maxY; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            Vector3 screenPoint(j + 0.5f, i + 0.5f, 0.0f);
            if (IsInTriangle(screenPoint, triangle[0].screenPos, triangle[1].screenPos, triangle[2].screenPos) &&
                !(j >= width || i >= height || j <= 0 || i <= 0)) // 条件保证不会出界
            {
                int index = GetIndex(j, i);

                DrawPointWithTriangleLerp(triangle, screenPoint, index, materialIndex, texName);
            }

        }
    }


}

/// <summary>
///  直接使用 y 轴在中间的点分割三角形进行绘制
/// </summary>
/// <param name="triangle"></param>
/// <param name="materialIndex"></param>
/// <param name="texName"></param>
void Scene::DrawTriangleWithMidPoint(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
{

    SortTriangle(triangle,false);

    // 仅仅绘制左边的三角形
    if ((triangle[2].screenPos.x - triangle[1].screenPos.x) < 1.0f)
    {
        DrawLeftTriangle(triangle[0],triangle[1],triangle[2],materialIndex, texName);
    }
    // 仅仅绘制右边的三角形
    else if ((triangle[1].screenPos.x - triangle[0].screenPos.x) < 1.0f)
    {
        DrawRightTriangle(triangle[0], triangle[1], triangle[2], materialIndex, texName);
    }
    // 两边都要绘制
    else
    {
        float t = (triangle[1].screenPos.x - triangle[0].screenPos.x )
            / (triangle[2].screenPos.x - triangle[0].screenPos.x);

        Vertex midPoint;
        LerpVertex(midPoint, triangle[2], triangle[0], t,false);


        DrawLeftTriangle(triangle[0], triangle[1], midPoint, materialIndex, texName);
        DrawRightTriangle(triangle[1], midPoint, triangle[2], materialIndex, texName);
    }

}




/// <summary>
///  使用包围盒初始化阴影贴图
/// </summary>
void Scene::BuildShadowWithBoundingBox(std::vector<Vertex>& triangle)
{

    // 包围盒
    int minX = (int)MathHelper::Min(MathHelper::Min(triangle[0].lightScreenPos.x, triangle[1].lightScreenPos.x),
        triangle[2].lightScreenPos.x);
    int minY = (int)MathHelper::Min(MathHelper::Min(triangle[0].lightScreenPos.y, triangle[1].lightScreenPos.y),
        triangle[2].lightScreenPos.y);

    int maxX = (int)MathHelper::Max(MathHelper::Max(triangle[0].lightScreenPos.x, triangle[1].lightScreenPos.x),
        triangle[2].lightScreenPos.x);
    int maxY = (int)MathHelper::Max(MathHelper::Max(triangle[0].lightScreenPos.y, triangle[1].lightScreenPos.y),
        triangle[2].lightScreenPos.y);


    // 遍历包围盒并绘制
    for (int i = minY; i <= maxY; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            Vector3 lightScreenPoint(j + 0.5f, i + 0.5f, 0.0f);
            if (IsInTriangle(lightScreenPoint, triangle[0].lightScreenPos,
                triangle[1].lightScreenPos, triangle[2].lightScreenPos) &&
                !(j >= width || i >= height || j <= 0 || i <= 0)) // 条件保证不会出界
            {
                int index = GetIndex(j, i);
                float alpha = 0, beat = 0, gamma = 0;

                ComputeBarycentric(triangle, lightScreenPoint.x, lightScreenPoint.y, alpha, beat, gamma, true);

                float nAlpha = alpha * triangle[0].lightScreenPos.z;
                float nBeat = beat * triangle[1].lightScreenPos.z;
                float nGamma = gamma * triangle[2].lightScreenPos.z;

                float depth =  (nAlpha + nBeat + nGamma);

                if (depth < shadowMap[index])
                {
                    shadowMap[index] = depth;
                }
            }
        }
    }
}

void Scene::BuildShadowWithMidPoint(std::vector<Vertex>& triangle)
{
    SortTriangle(triangle,true);

    bool isShadow = true;

    // 仅仅绘制左边的三角形
    if ((triangle[2].lightScreenPos.x - triangle[1].lightScreenPos.x) < 1.0f)
    {
        DrawLeftTriangle(triangle[0], triangle[1], triangle[2], isShadow);
    }
    // 仅仅绘制右边的三角形
    else if ((triangle[1].lightScreenPos.x - triangle[0].lightScreenPos.x) < 1.0f)
    {
        DrawRightTriangle(triangle[0], triangle[1], triangle[2], isShadow);
    }
    // 两边都要绘制
    else
    {
        float t = (triangle[1].lightScreenPos.x - triangle[0].lightScreenPos.x)
            / (triangle[2].lightScreenPos.x - triangle[0].lightScreenPos.x);

        Vertex midPoint;
        LerpVertex(midPoint, triangle[2], triangle[0], t, isShadow);


        DrawLeftTriangle(triangle[0], triangle[1], midPoint, isShadow);
        DrawRightTriangle(triangle[1], midPoint, triangle[2], isShadow);
    }

   

}




/// <summary>
///  绘制左边的三角形
///  按照 x 由小到大的顺序排序
///  v1.x < v2.x = v3.x
///  bool 用来确定是不是对阴影图的处理
/// </summary>
void Scene::DrawLeftTriangle(Vertex& v1, Vertex& v2, Vertex& v3, int& materialIndex, std::string& texName)
{
   
    for (float i = v1.screenPos.x; i <= v3.screenPos.x; i++)
    {
        float t = (i - v1.screenPos.x ) / (v3.screenPos.x - v1.screenPos.x);

        Vertex v12,v13;
        LerpVertex(v12, v2, v1, t,false);
        LerpVertex(v13, v3, v1, t,false);
        
        if (v13.screenPos.y > v12.screenPos.y)
        {
            Vertex temp = v13;
            v13 = v12;
            v12 = temp;
        }

        for (float j = v13.screenPos.y; j <= v12.screenPos.y; j++)
        {
            if (!(j >= width || i >= height || j < 0 || i < 0)) // 条件保证不会出界
            {
                float _t = (j - v13.screenPos.y) / (v12.screenPos.y - v13.screenPos.y);
                int index = GetIndex(i + 0.5f, j + 0.5f);

                Vertex point;
                LerpVertex(point,v12 , v13 , _t,false);
                DrawPointBilinearLerp(point, index, materialIndex, texName);
            }
        }

    }
}

/// <summary>
///  绘制右边的三角形
///  v1.x = v2.x < v3.x
///  bool 用来确定是不是对阴影图的处理
/// </summary>
void Scene::DrawRightTriangle(Vertex& v1, Vertex& v2, Vertex& v3, int& materialIndex, std::string& texName)
{

    for (float i = v1.screenPos.x; i <= v3.screenPos.x; i++)
    {
        float t = (i - v1.screenPos.x ) / (v3.screenPos.x - v1.screenPos.x);

        Vertex v13, v23;
        LerpVertex(v13, v3, v1, t,false);
        LerpVertex(v23, v3, v2, t,false);
       
        if (v13.screenPos.y > v23.screenPos.y)
        {
            Vertex temp = v13;
            v13 = v23;
            v23 = temp;
        }
        for (float j = v13.screenPos.y; j <= v23.screenPos.y; j++)
        {
            if (!(j >= width || i >= height || j < 0 || i < 0)) // 条件保证不会出界
            {
                float _t = (j - v13.screenPos.y) / (v23.screenPos.y - v13.screenPos.y);
                int index = GetIndex(i + 0.5f, j + 0.5f);

                Vertex point;
                LerpVertex(point, v23, v13, _t,false);
                DrawPointBilinearLerp(point, index, materialIndex, texName);
            }
        }

    }
}




/// <summary>
/// 用来处理阴影
/// </summary>
void Scene::DrawLeftTriangle(Vertex& v1, Vertex& v2, Vertex& v3, bool isShadow)
{
    for (float i = v1.lightScreenPos.x; i <= v3.lightScreenPos.x; i++)
    {
        float t = (i - v1.lightScreenPos.x) / (v3.lightScreenPos.x - v1.lightScreenPos.x);

        Vertex v12, v13;
        LerpVertex(v12, v2, v1, t, false);
        LerpVertex(v13, v3, v1, t, false);

        if (v13.lightScreenPos.y > v12.lightScreenPos.y)
        {
            Vertex temp = v13;
            v13 = v12;
            v12 = temp;
        }

        for (float j = v13.lightScreenPos.y; j <= v12.lightScreenPos.y; j++)
        {
            if (!(j >= width || i >= height || j < 0 || i < 0)) // 条件保证不会出界
            {
                float _t = (j - v13.lightScreenPos.y) / (v12.lightScreenPos.y - v13.lightScreenPos.y);
                int index = GetIndex(i + 0.5f, j + 0.5f);

                Vertex point;
                LerpVertex(point, v12, v13, _t, false);
                if (point.lightScreenPos.z < shadowMap[index])
                {
                    shadowMap[index] = point.lightScreenPos.z;
                }
            }
        }

    }
}

/// <summary>
/// 用来处理阴影
/// </summary>
void Scene::DrawRightTriangle(Vertex& v1, Vertex& v2, Vertex& v3, bool isShadow)
{
    for (float i = v1.screenPos.x; i <= v3.screenPos.x; i++)
    {
        float t = (i - v1.screenPos.x) / (v3.screenPos.x - v1.screenPos.x);

        Vertex v13, v23;
        LerpVertex(v13, v3, v1, t, false);
        LerpVertex(v23, v3, v2, t, false);

        if (v13.screenPos.y > v23.screenPos.y)
        {
            Vertex temp = v13;
            v13 = v23;
            v23 = temp;
        }
        for (float j = v13.screenPos.y; j <= v23.screenPos.y; j++)
        {
            if (!(j >= width || i >= height || j < 0 || i < 0)) // 条件保证不会出界
            {
                float _t = (j - v13.screenPos.y) / (v23.screenPos.y - v13.screenPos.y);
                int index = GetIndex(i + 0.5f, j + 0.5f);

                Vertex point;
                LerpVertex(point, v23, v13, _t, false);
                if (point.lightScreenPos.z < shadowMap[index])
                {
                    shadowMap[index] = point.lightScreenPos.z;
                }
            }
        }

    }
}






#pragma region 辅助函数

/// <summary>
/// 绘制三角形中的点
/// </summary>
/// <param name="triangle">三角形的三个点的数组</param>
/// <param name="screenPoint">点在屏幕上的位置</param>
/// <param name="index">点在帧缓冲中对应的索引，使用 GetIndex() 函数获取</param>
/// <param name="materialIndex">点的材质索引</param>
/// <param name="texName">点的贴图名称</param>
void Scene::DrawPointWithTriangleLerp(std::vector<Vertex>& triangle, Vector3& screenPoint,
    int& index, int& materialIndex, std::string& texName)
{
    float alpha = 0, beat = 0, gamma = 0;

    ComputeBarycentric(triangle, screenPoint.x, screenPoint.y, alpha, beat, gamma, false);


    float nAlpha = alpha / triangle[0].screenPos.z;
    float nBeat = beat / triangle[1].screenPos.z;
    float nGamma = gamma / triangle[2].screenPos.z;

    // 直接套用透视修正插值的结果
    float depth = 1 / (nAlpha + nBeat + nGamma);


    if (depth < depthBuffer[index])
    {
        depthBuffer[index] = depth;


        Vertex point;

        point.worldPos = triangle[0].worldPos * nAlpha + triangle[1].worldPos
            * nBeat + triangle[2].worldPos * nGamma;
        point.worldPos = point.worldPos * depth;
        point.worldPos.w = 1.0f;

        point.worldNormal = triangle[0].worldNormal * nAlpha + triangle[1].worldNormal
            * nBeat + triangle[2].worldNormal * nGamma;
        point.worldNormal = point.worldNormal * depth;



        if (texName == "" || mTextures[texName] == NULL)
        {
            point.color = triangle[0].color * alpha +
                triangle[1].color * beat +
                triangle[2].color * gamma;
        }
        else
        {
            point.texcoord = triangle[0].texcoord * nAlpha + triangle[1].texcoord * nBeat
                + triangle[2].texcoord * nGamma;
            point.texcoord = point.texcoord * depth;

            point.color = mTextures[texName]->GetColor(point.texcoord.u, point.texcoord.v);
        }

        CalcLight(point, materialIndex);

        if (CalcShadow(point.worldPos) && openShadow)
        {
            point.color = point.color * 0.1f;
        }
        frameBuffer[index] = point.color.GetIntColor();


    }

}

/// <summary>
///  不适用重心坐标插值，直接绘制点
/// </summary>
void Scene::DrawPointBilinearLerp(Vertex& point, int& index, int& materialIndex, std::string& texName)
{
    if (point.screenPos.z <= depthBuffer[index])
    {
        depthBuffer[index] = point.screenPos.z;
        if (!(texName == "" || mTextures[texName] == NULL) && useTexture)
        {
            point.color = mTextures[texName]->GetColor(point.texcoord.u, point.texcoord.v);
        }

        if (openLight)
        {
            CalcLight(point, materialIndex);
        }
      

        if (CalcShadow(point.worldPos) && openShadow)
        {
            point.color = point.color * 0.1f;
        }

        frameBuffer[index] = point.color.GetIntColor();
    }
}




/// <summary>
///  计算光照
///  光照可以在世界空间或者 摄像机空间进行计算
///  这里为了方便，直接在世界空间计算了。
/// </summary>
void Scene::CalcLight(Vertex& point, int& materialIndex)
{

    if (materialIndex != -1 && materialIndex < mMaterials.size() && mLights.size() > 0)
    {
        Vector4 kd = mMaterials[materialIndex].diffuse;
        Vector4 ks = mMaterials[materialIndex].fresnel;
        float gloss = mMaterials[materialIndex].gloss;

        Vector4 resColor(0, 0, 0, 0);
        Vector4 diff, spec;
        // 顶点世界坐标位置
        Vector3 pos = point.worldPos.toVector3();
        // 顶点世界坐标法线
        Vector3 normal = (point.worldNormal).Normalize();
        Vector4 vertexColor = point.color;


        for (int lIndex = 0; lIndex < mLights.size(); lIndex++)
        {

            Vector4 lightIntensity = mLights[lIndex].intensity;
            Vector3 dir = (mLights[lIndex].dir).Normalize();
            Vector3 viewDir = (camera.GetPosition() - pos).Normalize();

            diff = kd.Mul(lightIntensity) * MathHelper::Max(0.0f, normal.Dot(dir));
            diff = diff.Mul(vertexColor);

            Vector3 halfDir = (viewDir + dir).Normalize();
            spec = ks.Mul(lightIntensity) * std::pow(MathHelper::Max(0.0f,
                normal.Dot(halfDir)), gloss);
            spec = spec.Mul(vertexColor);

            resColor = diff + spec + resColor;
        }

        resColor = resColor + ambient_Color;
        resColor.x = resColor.x > 256 ? 255 : resColor.x;
        resColor.y = resColor.y > 256 ? 255 : resColor.y;
        resColor.z = resColor.z > 256 ? 255 : resColor.z;
        resColor.w = resColor.w > 256 ? 255 : resColor.w;

        point.color = resColor;
    }
}

/// <summary>
///  计算是否有阴影
/// </summary>
bool Scene::CalcShadow(Vector4 pos)
{
    // 转到了光源屏幕空间

    pos = lightView * pos;
    pos = lightProj * pos;

    pos = viewProt * pos;


    int index = GetIndex(pos.x + 0.5f, pos.y + 0.5f);


    if (pos.z > shadowMap[index])
    {
        return true;
    }
    return false;


}


/// <summary>
///  插值 target = t * a + (1-t) * b
/// </summary>
void Scene::LerpVertex(Vertex& target, Vertex& a, Vertex& b, float s,bool isShadow)
{
    if (isShadow)
    {
        target.lightScreenPos = a.lightScreenPos * s = b.lightScreenPos * (1 - s);
    }
    else
    {
        if (s <= 0.01f)
        {
            target = b;
            return;
        }
        if ((1 - s) <= 0.01f)
        {
            target = a;
            return;
        }

        float depth = 1 / ((s / a.screenPos.z) + ((1 - s) / b.screenPos.z));

        target.screenPos = a.screenPos * s + b.screenPos * (1 - s);
        target.screenPos.w = 1.0f;
        target.screenPos.z = depth;

        float t = (s * a.screenPos.z) / ((s * a.screenPos.z) + ((1-s) * b.screenPos.z));
        float invT = 1 - t;


        target.color = a.color * t + b.color * invT;
        target.worldNormal = a.worldNormal * t + b.worldNormal * invT;
        target.texcoord = a.texcoord * t + b.texcoord * invT;
        target.worldPos = a.worldPos * t + b.worldPos * invT;

        target.texcoord = target.texcoord * depth;
        target.worldPos = target.worldPos * depth;
        target.worldPos.w = 1.0f;
    }
}

/// <summary>
///  将三角形按照 x 方向有小到大排序
/// </summary>
/// <param name="triangle"></param>
void Scene::SortTriangle(std::vector<Vertex>& triangle,bool isShadow)
{
    if (isShadow)
    {
        if (triangle[0].lightScreenPos.x > triangle[1].lightScreenPos.x)
        {
            Vertex temp = triangle[0];
            triangle[0] = triangle[1];
            triangle[1] = temp;
        }
        if (triangle[0].lightScreenPos.x > triangle[2].lightScreenPos.x)
        {
            Vertex temp = triangle[0];
            triangle[0] = triangle[2];
            triangle[2] = temp;
        }
        if (triangle[1].lightScreenPos.x > triangle[2].lightScreenPos.x)
        {
            Vertex temp = triangle[1];
            triangle[1] = triangle[2];
            triangle[2] = temp;
        }
    }
    else
    {
        if (triangle[0].screenPos.x > triangle[1].screenPos.x)
        {
            Vertex temp = triangle[0];
            triangle[0] = triangle[1];
            triangle[1] = temp;
        }
        if (triangle[0].screenPos.x > triangle[2].screenPos.x)
        {
            Vertex temp = triangle[0];
            triangle[0] = triangle[2];
            triangle[2] = temp;
        }
        if (triangle[1].screenPos.x > triangle[2].screenPos.x)
        {
            Vertex temp = triangle[1];
            triangle[1] = triangle[2];
            triangle[2] = temp;
        }
    }
}

/// <summary>
///  判断点是不是在三角形内
/// </summary>
bool Scene::IsInTriangle(Vector3& point, Vector4& v_1, Vector4& v_2, Vector4& v_3)
{
    Vector3 v1(v_1.x, v_1.y,0);
    Vector3 v2(v_2.x, v_2.y,0);
    Vector3 v3(v_3.x, v_3.y,0);

    Vector3 v12 = v2 - v1;
    Vector3 v1_p = point - v1;

    Vector3 v23 = v3 - v2;
    Vector3 v2_p = point - v2;

    Vector3 v31 = v1 - v3;
    Vector3 v3_p = point - v3;

    Vector3 v1_res = v12.Cross(v1_p);
    Vector3 v2_res = v23.Cross(v2_p);
    Vector3 v3_res = v31.Cross(v3_p);

    float res_1 = v1_res.Dot(v2_res);
    float res_2 = v2_res.Dot(v3_res);
    float res_3 = v3_res.Dot(v1_res);

    if ((res_1 >= 0 && res_2 >= 0 && res_3 >= 0) ||
        (res_1 <= 0 && res_2 <= 0 && res_3 <= 0))
    {
        return  true;
    }

    return false;
}

/// <summary>
///  计算重心坐标插值
/// </summary>
void Scene::ComputeBarycentric(std::vector<Vertex> triangle, float x, float y, float& alpha, 
    float& beat, float& gamma,bool isLight = false)
{
    float xA = 0, xB = 0, xC = 0, yA = 0, yB = 0, yC = 0;
    if (!isLight)
    {
        xA = triangle[0].screenPos.x;
        xB = triangle[1].screenPos.x;
        xC = triangle[2].screenPos.x;
        yA = triangle[0].screenPos.y;
        yB = triangle[1].screenPos.y;
        yC = triangle[2].screenPos.y;
    }
    else
    {
        xA = triangle[0].lightScreenPos.x;
        xB = triangle[1].lightScreenPos.x;
        xC = triangle[2].lightScreenPos.x;
        yA = triangle[0].lightScreenPos.y;
        yB = triangle[1].lightScreenPos.y;
        yC = triangle[2].lightScreenPos.y;
    }
   

    // 按照公式计算的出来的东西
    alpha = ((x - xB) * (yB - yC) + (y - yB) * (xC - xB)) / ((xA - xB) * (yB - yC) + (yA - yB) * (xC - xB));
    beat = ((x - xC) * (yC - yA) + (y - yC) * (xA - xC)) / ((xB - xC) * (yC - yA) + (yB - yC) * (xA - xC));
    gamma = 1 - alpha - beat;


    // games101 给出的，实际就是原来公式的化简
   /* alpha = (x * (yB - yC) + (xC - xB) * y + xB * yC - xC * yB) / (xA * (yB - yC) + (xC - xB) * yA + xB * yC - xC * yB);
    beat = (x * (yC - yA) + (xA - xC) * y + xC * yA - xA * yC) / (xB * (yC - yA) + (xA - xC) * yB + xC * yA - xA * yC);
    gamma = (x * (yA - yB) + (xB - xA) * y + xA * yB - xB * yA) / (xC * (yA - yB) + (xB - xA) * yC + xA * yB - xB * yA);*/


}

/// <summary>
///  背面剔除,为正面时返回 true
/// </summary>
bool Scene::JudgeBackOrFront(std::vector<Vertex> triangle)
{
    Vector3 v1(triangle[0].screenPos.x, triangle[0].screenPos.y, 0.0f);
    Vector3 v2(triangle[1].screenPos.x, triangle[1].screenPos.y, 0.0f);
    Vector3 v3(triangle[2].screenPos.x, triangle[2].screenPos.y, 0.0f);


    Vector3 v12 = (v2 - v1);
    Vector3 v13 = (v3 - v1);

    if (v12.Cross(v13).z <= 0)
    {
        return true;
    }
    return false;

}

/// <summary>
///  获取屏幕索引
/// </summary>
long Scene::GetIndex(int x, int y)
{
    long all = width * height;

    // DX 的屏幕是从左上角开始的，
    // 这里我们希望从左下角开始，所以进行这样的变换
    long res = all - (y * width) + x;

    //long res = all - (y * width) + (height - x);
    if (res >= all || res < 0)
    {
        return 0;
    }
    return res;
}


#pragma endregion