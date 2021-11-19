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

    camera.SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 500.0f);


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
        DrawItem();
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
    l1.origin = Vector3(0, 10, -5);
    l1.intensity = Vector4(50.0f,50.0f,50.0f,0.5f);

    Light l2;
    l2.origin = Vector3(-5, -5, -10);
    l2.intensity = Vector4(10.0f, 10.0f, 10.0f, 0.5f);

    mLights.push_back(std::move(l1));
    mLights.push_back(std::move(l2));

    Vector3 lightTarget = Vector3(0, 0, 0) - l1.origin;
    Vector3 lightUp(0, 1, 0);
    Vector3 lightRight = lightTarget.Cross(lightUp);
    lightUp = lightRight.Cross(lightTarget);

    // Ĭ�ϵ�һ������Ϊ����Դ
    lightView = { lightRight.x, lightRight.y, lightRight.z, l1.origin.Dot(lightRight) * -1,
            lightUp.x, lightUp.y, lightUp.z, l1.origin.Dot(lightUp) * -1,
            lightTarget.x, lightTarget.y, lightTarget.z, l1.origin.Dot(lightTarget) * -1,
            0, 0, 0, 1 };

    float lightFovY = camera.GetFovY();
    float lightAspect = camera.GetAspect();
    float lightNear = 10;
    float lightFar = 3000;

    lightProj = { (1 / std::tan(lightFovY * 0.5f)) / lightAspect, 0, 0, 0,
        0, (1 / std::tan(lightFovY * 0.5f)), 0, 0,

        0, 0,  -1 * (lightNear + lightFar) / (lightFar - lightNear), 
        -1 * (2 * lightNear * lightFar) / (lightFar - lightNear),

        0, 0, 1, 0 };

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
            shadowMap[GetIndex(col, row)] = 0.0f;
        }
    }
    ShadowShader();
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
            depthBuffer[GetIndex(col, row)] = 0.0f;
        }
    }
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
///  ������Ⱦ��
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

            //Vertex v1 = i->second->vertexs[i->second->indexs[j]];
            //Vertex v2 = i->second->vertexs[i->second->indexs[j + 1]];
            //Vertex v3 = i->second->vertexs[i->second->indexs[j + 2]];


            #pragma region ʹ�ð�Χ�н��л��ƵĴ���
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 1]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 2]]);
            // �����޳�
            if (JudgeBackOrFront(vertex_triangle))
            {
                FragmentShaderWithBoundingBox(vertex_triangle, i->second->materialIndex, i->second->textureIndex);
            }

            #pragma endregion


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
/// ����������
/// ֱ��ʹ�������εİ�Χ�н����жϻ��ƣ����Ļ�Ƚϴ󣬲��Ƽ�ʹ��
/// </summary>
void Scene::FragmentShaderWithBoundingBox(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
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

                DrawPoint(triangle, screenPoint, index, materialIndex, texName);
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
void Scene::FragmentShaderWithTriangle(std::vector<Vertex>& triangle, int& materialIndex, std::string& texName)
{

}





/// <summary>
///  ��Ӱ��ͼ����ɫ����
/// ����������Ⱦһ�鳡�����������ڹ�Դ���ӽ���
/// </summary>
void Scene::ShadowShader()
{
    for (auto i = mGeos.begin(); i != mGeos.end(); i++)
    {
        for (int j = 0; j < i->second->vertexs.size(); j++)
        {
            Vector4 pos = i->second->vertexs[j].modelPos;

            i->second->vertexs[j].worldPos = i->second->worldMatrix * pos;

            pos = i->second->vertexs[j].worldPos;

            pos = lightView * pos;

            pos = lightProj * pos;

            float w = 1 / pos.w;
            pos = pos * w;

            pos = viewProt * pos;

            i->second->vertexs[j].lightScreenPos = pos;
        }
    }

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
            
            ShadowFragmentShadow(vertex_triangle);
        }
    }
}

/// <summary>
///  ������Ӱ��ͼ
/// </summary>
void Scene::ShadowFragmentShadow(std::vector<Vertex>& triangle)
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

                ComputeBarycentric(triangle, lightScreenPoint.x, lightScreenPoint.y, alpha, beat, gamma,true);

                float nAlpha = alpha / triangle[0].lightScreenPos.z;
                float nBeat = beat / triangle[1].lightScreenPos.z;
                float nGamma = gamma / triangle[2].lightScreenPos.z;

                // ֱ������͸��������ֵ�Ľ��
                float depth = 1 / (nAlpha + nBeat + nGamma);

                if (depth < shadowMap[index])
                {
                    shadowMap[index] = depth;
                }
            }
        }
    }
}



#pragma region ��������


/// <summary>
///  �����������еĵ�
/// </summary>
void Scene::DrawPoint(std::vector<Vertex>& triangle, Vector3& screenPoint, 
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

        // ֱ�Ӽӿ��ܻᵼ�����������ֿ���
        //// ���߰�ԭ�����������͸�Ϊ double Ҳ�У�����û��Ҫ
        //Vector2 x = triangle[0].texcoord * nAlpha;
        //Vector2 y = triangle[1].texcoord * nBeat;
        //Vector2 z = triangle[2].texcoord * nGamma;
        //point.texcoord = (x + y + z);
        //point.texcoord = point.texcoord  * depth;


        Vector2 x = triangle[0].texcoord * alpha;
        Vector2 y = triangle[1].texcoord * beat;
        Vector2 z = triangle[2].texcoord * gamma;
        point.texcoord = (x + y + z);


        if (texName == "" || mTextures[texName] == NULL)
        {
            point.color = triangle[0].color * alpha +
                triangle[1].color * beat +
                triangle[2].color * gamma;
        }
        else
        {
            point.color = mTextures[texName]->GetColor(point.texcoord.u, point.texcoord.v);
        }

        CalcLight(point, materialIndex);

        if (CalcShadow(point.worldPos))
        {
            point.color = point.color * 0.1f;
        }
        frameBuffer[index] = point.color.GetIntColor();
        

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

    float w = 1 / pos.w;
    pos = pos * w;

    pos = viewProt * pos;

    int p = pos.z * 100;
    int shadow = shadowMap[GetIndex(pos.x, pos.y)] * 100;

    if (p >= shadow)
    {
        return true;
    }
    return false;

    
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
///  ��ȡ��Ļ����
/// </summary>
long Scene::GetIndex(int x, int y)
{
    long all = width * height;

    // DX ����Ļ�Ǵ����Ͻǿ�ʼ�ģ�
    // ��������ϣ�������½ǿ�ʼ�����Խ��������ı任

    long res = all - (y * width) + (height - x);
    if (res >= all || res < 0)
    {
        return 0;
    }
    return res;
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
    //alpha = ((x - xB) * (yB - yC) + (y - yB) * (xC - xB)) / ((xA - xB) * (yB - yC) + (yA - yB) * (xC - xB));
    //beat = ((x - xC) * (yC - yA) + (y - yC) * (xA - xC)) / ((xB - xC) * (yC - yA) + (yB - yC) * (xA - xC));
    //gamma = 1 - alpha - beat;


    // games101 �����ģ�ʵ�ʾ���ԭ����ʽ�Ļ���
    alpha = (x * (yB - yC) + (xC - xB) * y + xB * yC - xC * yB) / (xA * (yB - yC) + (xC - xB) * yA + xB * yC - xC * yB);
    beat = (x * (yC - yA) + (xA - xC) * y + xC * yA - xA * yC) / (xB * (yC - yA) + (xA - xC) * yB + xC * yA - xA * yC);
    gamma = (x * (yA - yB) + (xB - xA) * y + xA * yB - xB * yA) / (xC * (yA - yB) + (xB - xA) * yC + xA * yB - xB * yA);


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


#pragma endregion