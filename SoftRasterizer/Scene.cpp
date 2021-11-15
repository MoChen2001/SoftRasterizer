#include "Scene.h"


// 初始化场景，初始化长度高度以及帧缓冲和深度缓冲
void Scene::InitScene(int w, int h, unsigned int*& frameBuffer)
{
    width = w;
    height = h;


    Matrix4x4 tempViewProt = 
    { (float)width / 2, 0, 0,  (float)width / 2,
    0, (float)height / 2, 0, (float)height / 2,
    0, 0, 1.0f, 0.0f,
    0, 0, 0, 1.0f };


    viewProt = tempViewProt;

    this->frameBuffer = frameBuffer;
    depthBuffer.reset(new float[w * h]);

    BuildCamera();
    BuildRendreItem();
    
}

/// <summary>
///  初始化摄像机
/// </summary>
void Scene::BuildCamera()
{
    camera.SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 500.0f);

    camera.SetPosition(0,0,-1);

    Vector3 target(0, 0, 0);
    Vector3 up(0, 1, 0);

    Vector3 right = target.Cross(up);
    up = right.Cross(target);

    camera.SetTarget(target);
    camera.SetRight(right);
    camera.SetUp(up);


}


/// <summary>
///  初始化渲染项
/// </summary>
void Scene::BuildRendreItem()
{
    auto triangle = std::make_unique<Geometry>();
    Vertex v_1, v_2, v_3,v_4,v_5,v_6;

    v_1.pos = Vector4(0.0f, 3.0f, 20.0f, 1.0f);
    v_2.pos = Vector4(-3.0f, 0.0f,20.0f, 1.0f);
    v_3.pos = Vector4(3.0f, 0.0f, 20.0f, 1.0f);

    v_4.pos = Vector4(0.0f, 5.0f, 21.0f, 1.0f);
    v_5.pos = Vector4(-3.0f, 0.0f, 21.0f, 1.0f);
    v_6.pos = Vector4(3.0f, 0.0f, 21.0f, 1.0f);

    v_1.color = Vector4(255, 0, 0, 0);
    v_2.color = Vector4(0, 255, 0, 0);
    v_3.color = Vector4(0, 0, 255, 0);
    v_4.color = Vector4(0, 255, 0, 0);
    v_5.color = Vector4(0, 255, 0, 0);
    v_6.color = Vector4(0, 255, 0, 0);

    triangle->indexs = { 0,1,2,
    3,4,5
    };

    triangle->vertexs.push_back(v_1);
    triangle->vertexs.push_back(v_2);
    triangle->vertexs.push_back(v_3);
    triangle->vertexs.push_back(v_4);
    triangle->vertexs.push_back(v_5);
    triangle->vertexs.push_back(v_6);

    triangle->worldMatrix = Matrix4x4{ 1.0f,0.0f,0.0f,0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f ,1.0f };


    mRenderItem.mGeos["triangle"] = std::move(triangle);


}


/// <summary>
///  变换渲染项到齐次裁剪空间 同时进行屏幕映射
/// </summary>
void Scene::TransformItem()
{
    int times = 1;
    for (auto i = mRenderItem.mGeos.begin(); i != mRenderItem.mGeos.end(); i++)
    {
        times++;
        for (int j = 0; j < i->second->vertexs.size(); j++)
        {
            Vector4 pos = i->second->vertexs[j].pos;

            pos = i->second->worldMatrix * pos;

            pos = camera.GetMyView() * pos;

            pos = camera.GetMyProj() * pos;

            float w = 1 / pos.w;
            pos = pos * w;

            
            pos = viewProt * pos;

            i->second->vertexs[j].pos = pos;
        }
    }
}

/// <summary>
///  
/// </summary>
void Scene::DrawItem()
{
    TransformItem();
    for (auto i = mRenderItem.mGeos.begin(); i != mRenderItem.mGeos.end(); i++)
    {

        for (int j = 0; j < i->second->indexs.size(); j += 3)
        {
            std::vector<Vertex> vertex_triangle;
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 1]]);
            vertex_triangle.push_back(i->second->vertexs[i->second->indexs[j + 2]]);

            DrawTriangle(vertex_triangle);
        }
    }
}



/// <summary>
///  更新帧
/// </summary>
void Scene::UpdateFrame()
{
    ClearFrameBuffer();
    DrawItem();


}




// 清理帧缓冲
void Scene::ClearFrameBuffer()
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int idx = row * width + col;
            // 默认背景色浅蓝 R123 G195 B221
            frameBuffer[GetIndex(col,row)] = bg_color.GetIntColor();
            // 深度缓冲区 1.0f
            depthBuffer[GetIndex(col, row)] = 0.0f;
        }
    }
}



/// <summary>
/// 绘制三角形
/// </summary>
void Scene::DrawTriangle(std::vector<Vertex> triangle)
{


    // 包围盒
    int minX = (int)std::min(std::min(triangle[0].pos.x, triangle[1].pos.x), triangle[2].pos.x);
    int minY = (int)std::min(std::min(triangle[0].pos.y, triangle[1].pos.y), triangle[2].pos.y);

    int maxX = (int)std::max(std::max(triangle[0].pos.x, triangle[1].pos.x), triangle[2].pos.x);
    int maxY = (int)std::max(std::max(triangle[0].pos.y, triangle[1].pos.y), triangle[2].pos.y);


    // 遍历包围盒并绘制
    for (int i = minY; i <= maxY; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            Vector3 point(j + 0.5f, i + 0.5f, 0.0f);
            if (IsInTriangle(point, triangle[0].pos, triangle[1].pos, triangle[2].pos) &&
                !(j >= width || i >= height || j <= 0 || i <= 0)) // 条件保证不会出界
            {
                int index = GetIndex(j, i);
                float alpha = 0, beat = 0, gamma = 0;

                ComputeBarycentric2D(triangle, point.x , point.y, alpha, beat, gamma);


                float z_interpolated = alpha * triangle[0].pos.z  / triangle[0].pos. w 
                    + beat * triangle[1].pos.z / triangle[1].pos.w + gamma * triangle[2].pos.z / triangle[2].pos.w;


                if (z_interpolated < depthBuffer[index])
                {
                    depthBuffer[index] = z_interpolated;

                    frameBuffer[index] = bg_color.GetIntColor();

                    // 颜色插值
                    Vector4 color = triangle[0].color * alpha + triangle[1].color * beat + triangle[2].color * gamma;
                    frameBuffer[index] = color.GetIntColor();
                }

            }
           

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



long Scene::GetIndex(int x, int y)
{
    long all = width * height;

    // DX 的屏幕是从左上角开始的，
    // 这里我们希望从左下角开始，所以进行这样的变换
    //int res = all - (y * width) + x;

    long res = all - (y * width) + x;
    if (res >= all || res < 0)
    {
        return 0;
    }
    return res;
}


/// <summary>
///  计算重心坐标插值
/// </summary>
void Scene::ComputeBarycentric2D(std::vector<Vertex> triangle, float x, float y, float& alpha, float& beat, float& gamma)
{
    float xA = triangle[0].pos.x;
    float xB = triangle[1].pos.x;
    float xC = triangle[2].pos.x;
    float yA = triangle[0].pos.y;
    float yB = triangle[1].pos.y;
    float yC = triangle[2].pos.y;

    // 按照公式计算的出来的东西
    //alpha = ((x - xB) * (yB - yC) + (y - yB) * (xC - xB)) / ((xA - xB) * (yB - yC) + (yA - yB) * (xC - xB));
    //beat = ((x - xC) * (yC - yA) + (y - yC) * (xA - xC)) / ((xB - xC) * (yC - yA) + (yB - yC) * (xA - xC));
    //gamma = 1 - alpha - beat;


    // games101 给出的，实际就是原来公式的化简
    alpha = (x * (yB - yC) + (xC - xB) * y + xB * yC - xC * yB) / (xA * (yB - yC) + (xC - xB) * yA + xB * yC - xC * yB);
    beat = (x * (yC - yA) + (xA - xC) * y + xC * yA - xA * yC) / (xB * (yC - yA) + (xA - xC) * yB + xC * yA - xA * yC);
    gamma = (x * (yA - yB) + (xB - xA) * y + xA * yB - xB * yA) / (xC * (yA - yB) + (xB - xA) * yC + xA * yB - xB * yA);


}