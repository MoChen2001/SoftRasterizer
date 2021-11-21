#include "Scene.h"


/// <summary>
/// ��ʼ����������ʼ�����ȸ߶��Լ�֡�������Ȼ���
/// </summary>
void Scene::InitScene(int w, int h, unsigned int*& frameBuffer)
{
    width = w;
    height = h;


    viewProt = { (float)width / 2, 0, 0,  (float)width / 2,
    0, (float)height / 2, 0, (float)height / 2,
    // Ϊ�˱��ڼ���
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
///  ����֡
/// </summary>
void Scene::UpdateSceneData()
{
    UpdateCamera();

}

/// <summary>
///  ���Ƴ���
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
///  ��������
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
///  ������Դ
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

    // Ĭ�ϵ�һ������Ϊ����Դ
    // ��Դ��λ��ȡ������Χ�������λ�ã�
    // ������Χ��뾶Ӧ��Ϊ 3 
    lightView = { lightRight.x, lightRight.y, lightRight.z, lightPos.Dot(lightRight) * -1,
            lightUp.x, lightUp.y, lightUp.z,  lightPos.Dot(lightUp) * -1,
            lightTarget.x, lightTarget.y, lightTarget.z , lightPos.Dot(lightUp) * -1,
            0, 0, 0, 1 };

    Vector4 center(0, 0, 0, 0);

    center = lightView * center;

    float n = center.z - 3.0f;
    float f = center.z + 3.0f;

    /// <summary>
    ///  ƽ�й������ͶӰ��Ӧ���ǳ�����Χ�е����ŵ� ��οռ�
    /// </summary>
    lightProj = { 1.0f / 3.0f , 0, 0, 0,
        0,  1.0f / 3.0f, 0, 0,
        0, 0,  1.0f / 6.0f,   -n / 6.0f,
        0, 0, 0, 1 };

}

/// <summary>
///  ��������
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
///  ��ʼ����Ⱦ��
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
///  ������Ӱ��ͼ
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
///  ���������
/// </summary>
void Scene::UpdateCamera()
{
    Vector3 mEyePos;

    // ������
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
/// ����֡����
/// </summary>
void Scene::ClearFrameBuffer()
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int idx = row * width + col;
            // Ĭ�ϱ���ɫǳ�� R123 G195 B221
            frameBuffer[GetIndex(col, row)] = bg_color.GetIntColor();
            // ��Ȼ����� 1.0f
            depthBuffer[GetIndex(col, row)] = 1.0f;
        }
    }

}

/// <summary>
///  ��Ҫ���ж�����ɫ�������飬��������һ���ӿڱ任�Լ�͸�ӳ���
///  ����Ϊ�˷���֮�����������ʹ��
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
///  ƬԪ��ɫ��
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

            // �����޳�
            if (JudgeBackOrFront(vertex_triangle))
            {
                // ��Χ�л���
                //DrawTriangleWithBoundingBox(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
                
                // ֱ�ӻ���
                DrawTriangleWithMidPoint(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
            }



        }
    }
}




/// <summary>
///  ��Ӱ��ͼ����ɫ����
/// ����������Ⱦһ�鳡�����������ڹ�Դ���ӽ���
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
///  ������Ӱ��ͼ
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
/// ����������
/// ֱ��ʹ�������εİ�Χ�н����жϻ��ƣ����Ļ�Ƚϴ󣬲��Ƽ�ʹ��
/// </summary>
void Scene::DrawTriangleWithBoundingBox(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
{

    // ��Χ��
    int minX = (int)MathHelper::Min(MathHelper::Min(triangle[0].screenPos.x, triangle[1].screenPos.x), triangle[2].screenPos.x);
    int minY = (int)MathHelper::Min(MathHelper::Min(triangle[0].screenPos.y, triangle[1].screenPos.y), triangle[2].screenPos.y);

    int maxX = (int)MathHelper::Max(MathHelper::Max(triangle[0].screenPos.x, triangle[1].screenPos.x), triangle[2].screenPos.x);
    int maxY = (int)MathHelper::Max(MathHelper::Max(triangle[0].screenPos.y, triangle[1].screenPos.y), triangle[2].screenPos.y);


    // ������Χ�в�����
    for (int i = minY; i <= maxY; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            Vector3 screenPoint(j + 0.5f, i + 0.5f, 0.0f);
            if (IsInTriangle(screenPoint, triangle[0].screenPos, triangle[1].screenPos, triangle[2].screenPos) &&
                !(j >= width || i >= height || j <= 0 || i <= 0)) // ������֤�������
            {
                int index = GetIndex(j, i);

                DrawPointWithTriangleLerp(triangle, screenPoint, index, materialIndex, texName);
            }

        }
    }


}

/// <summary>
///  ֱ��ʹ�� y �����м�ĵ�ָ������ν��л���
/// </summary>
/// <param name="triangle"></param>
/// <param name="materialIndex"></param>
/// <param name="texName"></param>
void Scene::DrawTriangleWithMidPoint(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
{

    SortTriangle(triangle,false);

    // ����������ߵ�������
    if ((triangle[2].screenPos.x - triangle[1].screenPos.x) < 1.0f)
    {
        DrawLeftTriangle(triangle[0],triangle[1],triangle[2],materialIndex, texName);
    }
    // ���������ұߵ�������
    else if ((triangle[1].screenPos.x - triangle[0].screenPos.x) < 1.0f)
    {
        DrawRightTriangle(triangle[0], triangle[1], triangle[2], materialIndex, texName);
    }
    // ���߶�Ҫ����
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
///  ʹ�ð�Χ�г�ʼ����Ӱ��ͼ
/// </summary>
void Scene::BuildShadowWithBoundingBox(std::vector<Vertex>& triangle)
{

    // ��Χ��
    int minX = (int)MathHelper::Min(MathHelper::Min(triangle[0].lightScreenPos.x, triangle[1].lightScreenPos.x),
        triangle[2].lightScreenPos.x);
    int minY = (int)MathHelper::Min(MathHelper::Min(triangle[0].lightScreenPos.y, triangle[1].lightScreenPos.y),
        triangle[2].lightScreenPos.y);

    int maxX = (int)MathHelper::Max(MathHelper::Max(triangle[0].lightScreenPos.x, triangle[1].lightScreenPos.x),
        triangle[2].lightScreenPos.x);
    int maxY = (int)MathHelper::Max(MathHelper::Max(triangle[0].lightScreenPos.y, triangle[1].lightScreenPos.y),
        triangle[2].lightScreenPos.y);


    // ������Χ�в�����
    for (int i = minY; i <= maxY; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            Vector3 lightScreenPoint(j + 0.5f, i + 0.5f, 0.0f);
            if (IsInTriangle(lightScreenPoint, triangle[0].lightScreenPos,
                triangle[1].lightScreenPos, triangle[2].lightScreenPos) &&
                !(j >= width || i >= height || j <= 0 || i <= 0)) // ������֤�������
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

    // ����������ߵ�������
    if ((triangle[2].lightScreenPos.x - triangle[1].lightScreenPos.x) < 1.0f)
    {
        DrawLeftTriangle(triangle[0], triangle[1], triangle[2], isShadow);
    }
    // ���������ұߵ�������
    else if ((triangle[1].lightScreenPos.x - triangle[0].lightScreenPos.x) < 1.0f)
    {
        DrawRightTriangle(triangle[0], triangle[1], triangle[2], isShadow);
    }
    // ���߶�Ҫ����
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
///  ������ߵ�������
///  ���� x ��С�����˳������
///  v1.x < v2.x = v3.x
///  bool ����ȷ���ǲ��Ƕ���Ӱͼ�Ĵ���
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
            if (!(j >= width || i >= height || j < 0 || i < 0)) // ������֤�������
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
///  �����ұߵ�������
///  v1.x = v2.x < v3.x
///  bool ����ȷ���ǲ��Ƕ���Ӱͼ�Ĵ���
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
            if (!(j >= width || i >= height || j < 0 || i < 0)) // ������֤�������
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
/// ����������Ӱ
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
            if (!(j >= width || i >= height || j < 0 || i < 0)) // ������֤�������
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
/// ����������Ӱ
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
            if (!(j >= width || i >= height || j < 0 || i < 0)) // ������֤�������
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






#pragma region ��������

/// <summary>
/// �����������еĵ�
/// </summary>
/// <param name="triangle">�����ε������������</param>
/// <param name="screenPoint">������Ļ�ϵ�λ��</param>
/// <param name="index">����֡�����ж�Ӧ��������ʹ�� GetIndex() ������ȡ</param>
/// <param name="materialIndex">��Ĳ�������</param>
/// <param name="texName">�����ͼ����</param>
void Scene::DrawPointWithTriangleLerp(std::vector<Vertex>& triangle, Vector3& screenPoint,
    int& index, int& materialIndex, std::string& texName)
{
    float alpha = 0, beat = 0, gamma = 0;

    ComputeBarycentric(triangle, screenPoint.x, screenPoint.y, alpha, beat, gamma, false);


    float nAlpha = alpha / triangle[0].screenPos.z;
    float nBeat = beat / triangle[1].screenPos.z;
    float nGamma = gamma / triangle[2].screenPos.z;

    // ֱ������͸��������ֵ�Ľ��
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
///  ���������������ֵ��ֱ�ӻ��Ƶ�
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
///  �������
///  ���տ���������ռ���� ������ռ���м���
///  ����Ϊ�˷��㣬ֱ��������ռ�����ˡ�
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
        // ������������λ��
        Vector3 pos = point.worldPos.toVector3();
        // �����������귨��
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
///  �����Ƿ�����Ӱ
/// </summary>
bool Scene::CalcShadow(Vector4 pos)
{
    // ת���˹�Դ��Ļ�ռ�

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
///  ��ֵ target = t * a + (1-t) * b
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
///  �������ΰ��� x ������С��������
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
///  �жϵ��ǲ�������������
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
///  �������������ֵ
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
   

    // ���չ�ʽ����ĳ����Ķ���
    alpha = ((x - xB) * (yB - yC) + (y - yB) * (xC - xB)) / ((xA - xB) * (yB - yC) + (yA - yB) * (xC - xB));
    beat = ((x - xC) * (yC - yA) + (y - yC) * (xA - xC)) / ((xB - xC) * (yC - yA) + (yB - yC) * (xA - xC));
    gamma = 1 - alpha - beat;


    // games101 �����ģ�ʵ�ʾ���ԭ����ʽ�Ļ���
   /* alpha = (x * (yB - yC) + (xC - xB) * y + xB * yC - xC * yB) / (xA * (yB - yC) + (xC - xB) * yA + xB * yC - xC * yB);
    beat = (x * (yC - yA) + (xA - xC) * y + xC * yA - xA * yC) / (xB * (yC - yA) + (xA - xC) * yB + xC * yA - xA * yC);
    gamma = (x * (yA - yB) + (xB - xA) * y + xA * yB - xB * yA) / (xC * (yA - yB) + (xB - xA) * yC + xA * yB - xB * yA);*/


}

/// <summary>
///  �����޳�,Ϊ����ʱ���� true
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
///  ��ȡ��Ļ����
/// </summary>
long Scene::GetIndex(int x, int y)
{
    long all = width * height;

    // DX ����Ļ�Ǵ����Ͻǿ�ʼ�ģ�
    // ��������ϣ�������½ǿ�ʼ�����Խ��������ı任
    long res = all - (y * width) + x;

    //long res = all - (y * width) + (height - x);
    if (res >= all || res < 0)
    {
        return 0;
    }
    return res;
}


#pragma endregion