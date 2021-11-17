#include "Scene.h"


/// <summary>
/// 初始化场景，初始化长度高度以及帧缓冲和深度缓冲
/// </summary>
void Scene::InitScene(int w, int h, unsigned int*& frameBuffer)
{
    width = w;
    height = h;


    Matrix4x4 tempViewProt = 
    { (float)width / 2, 0, 0,  (float)width / 2,
    0, (float)height / 2, 0, (float)height / 2,
    0, 0, 1.0f, 0.0f,
    0, 0, 0, 1.0f };

    camera.SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 500.0f);

    viewProt = tempViewProt;

    this->frameBuffer = frameBuffer;
    depthBuffer.reset(new float[w * h]);

    BuildLight();
    BuildMaterial();
    BuildTexture();
    BuildRendreItem();
    UpdateCamera();
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
    ClearFrameBuffer();
    VertexShader();
    DrawItem();
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
    l1.origin = Vector3(10, 10, 0);
    l1.intensity = Vector4(50.0f,50.0f,50.0f,0.5f);

    Light l2;
    l2.origin = Vector3(-5, -5, -10);
    l2.intensity = Vector4(10.0f, 10.0f, 10.0f, 0.5f);

    mLights.push_back(std::move(l1));
    mLights.push_back(std::move(l2));
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
            depthBuffer[GetIndex(col, row)] = 0.0f;
        }
    }
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
            Vector4 pos = i->second->vertexs[j].worldPos;

            pos = i->second->worldMatrix * pos;

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
///  绘制渲染项
/// </summary>
void Scene::DrawItem()
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
                FragmentShader(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
            }
        }
    }
}


/// <summary>
/// 绘制三角形
/// </summary>
void Scene::FragmentShader(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
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
                float alpha = 0, beat = 0, gamma = 0;

                ComputeBarycentric(triangle, screenPoint.x, screenPoint.y, alpha, beat, gamma);

                float w_interpolated = 1 / (alpha * triangle[0].screenPos.w + beat * triangle[1].screenPos.w
                    + triangle[2].screenPos.w * gamma);

                // 透视修正插值
                float z_interpolated = alpha * triangle[0].screenPos.z / triangle[0].screenPos.w
                    + beat * triangle[1].screenPos.z / triangle[1].screenPos.w +
                    gamma * triangle[2].screenPos.z / triangle[2].screenPos.w;
                z_interpolated *= w_interpolated;

                if (z_interpolated < depthBuffer[index])
                {
                    depthBuffer[index] = z_interpolated;
                    

                    Vertex point;
                    point.worldPos = triangle[0].worldPos * alpha + triangle[1].worldPos
                        * beat + triangle[2].worldPos * gamma;
                    point.worldNormal = triangle[0].worldNormal * alpha + triangle[1].worldNormal 
                        * beat + triangle[2].worldNormal * gamma;

                    // 直接加可能会导致溢出，这里分开加
                    // 或者把原来的数据类型改为 double 也行，不过没必要
                    Vector2 x = triangle[0].texcoord * alpha;
                    Vector2 y = triangle[1].texcoord * beat;
                    Vector2 z = triangle[2].texcoord * gamma;
                    point.texcoord = x + y + z;

                    //point.texcoord = (triangle[0].texcoord * alpha) 
                    //    + (triangle[1].texcoord * beat )
                    //    + (triangle[2].texcoord * gamma);


                    point.texcoord = point.texcoord * w_interpolated;

                    if (texName == "" || mTextures[texName] == NULL)
                    {
                        point.color = triangle[0].color * alpha +
                            triangle[1].color * beat
                            + triangle[2].color * gamma;
                    }
                    else
                    {
                        int h = mTextures[texName]->height;
                        int w = mTextures[texName]->width;

                        int u = (int)(point.texcoord.u * h);
                        int v = (int)(point.texcoord.v * w);

                        point.color = mTextures[texName]->GetColor(u,v);
                    }
                    
                    CalcLight(point, materialIndex);

                    frameBuffer[index] = point.color.GetIntColor();
                }

            }


        }
    }


}



#pragma region 辅助函数

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
            Vector3 lightPos = mLights[lIndex].origin;

            float atten = (point.worldPos.toVector3() - lightPos).Distance();
            atten = 1 / atten;
            Vector4 lightIntensity = mLights[lIndex].intensity;
            Vector3 dir = (lightPos - pos).Normalize();
            Vector3 viewDir = (camera.GetPosition() - pos).Normalize();

            diff = kd.Mul(lightIntensity) * MathHelper::Max(0.0f, normal.Dot(dir)) * atten;
            diff = diff.Mul(vertexColor);

            Vector3 halfDir = (viewDir + dir).Normalize();
            spec = ks.Mul(lightIntensity) * std::pow(MathHelper::Max(0.0f,
                normal.Dot(halfDir)), gloss) * atten;
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
///  获取屏幕索引
/// </summary>
long Scene::GetIndex(int x, int y)
{
    long all = width * height;

    // DX 的屏幕是从左上角开始的，
    // 这里我们希望从左下角开始，所以进行这样的变换

    long res = all - (y * width) + (height - x);
    if (res >= all || res < 0)
    {
        return 0;
    }
    return res;
}


/// <summary>
///  计算重心坐标插值
/// </summary>
void Scene::ComputeBarycentric(std::vector<Vertex> triangle, float x, float y, float& alpha, float& beat, float& gamma)
{
    float xA = triangle[0].screenPos.x;
    float xB = triangle[1].screenPos.x;
    float xC = triangle[2].screenPos.x;
    float yA = triangle[0].screenPos.y;
    float yB = triangle[1].screenPos.y;
    float yC = triangle[2].screenPos.y;

    // 按照公式计算的出来的东西
    //alpha = ((x - xB) * (yB - yC) + (y - yB) * (xC - xB)) / ((xA - xB) * (yB - yC) + (yA - yB) * (xC - xB));
    //beat = ((x - xC) * (yC - yA) + (y - yC) * (xA - xC)) / ((xB - xC) * (yC - yA) + (yB - yC) * (xA - xC));
    //gamma = 1 - alpha - beat;


    // games101 给出的，实际就是原来公式的化简
    alpha = (x * (yB - yC) + (xC - xB) * y + xB * yC - xC * yB) / (xA * (yB - yC) + (xC - xB) * yA + xB * yC - xC * yB);
    beat = (x * (yC - yA) + (xA - xC) * y + xC * yA - xA * yC) / (xB * (yC - yA) + (xA - xC) * yB + xC * yA - xA * yC);
    gamma = (x * (yA - yB) + (xB - xA) * y + xA * yB - xB * yA) / (xC * (yA - yB) + (xB - xA) * yC + xA * yB - xB * yA);


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


#pragma endregion