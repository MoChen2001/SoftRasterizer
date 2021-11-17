#pragma once
#include "VectorX.h"
#include <iostream>
#include <vector>


/// <summary>
///  顶点
/// </summary>
struct Vertex
{
public:
	Vertex() {}
	Vertex(float px, float py , float pz,float pw,
		float nx, float ny, float nz,
		float cx, float cy, float cz, float cw,
		float tx, float ty):
		worldPos(px,py,pz,pw),worldNormal(nx,ny,nz), color(cx,cy,cz,cw),
	texcoord(tx,ty){}


	~Vertex() {}


public:
	Vector2 texcoord;
	Vector4 worldPos;
	Vector3 worldNormal;
	Vector4 screenPos;
	Vector4 color;
};




/// <summary>
/// 几何类, 总的抽象类，
/// 三角形顺序以逆序为前向
/// </summary>
class Geometry
{
public:
	Geometry() : materialIndex(-1){}
	~Geometry() {}
	Geometry(const Geometry& rhs) = delete;
	Geometry& operator=(const Geometry& rhs) = delete;
	
public:
	
	Matrix4x4 worldMatrix;
	std::vector<Vertex> vertexs;
	std::vector<int> indexs;
	int materialIndex;
	std::string textureIndex;
};



class Cube : public Geometry
{
private:
	Vector3 origin;
	float longer;
	float width;
	float height;

public:
	// 定义朝向 Z 轴负向为前平面
	Cube() :origin(0, 0, 0), longer(1), width(1), height(1) 
	{
		// 上平面
		Vertex v1(-1,1,1,1,	0,1,0,	255,0,0,0,	0,0);  
		Vertex v2(-1,1,-1,1,	0,1,0,	0,255,0,0,	1,0);
		Vertex v3(1,1,-1,1,	0,1,0,	0,0,255,0,	1,1);
		Vertex v4(1,1,1,1,	0,1,0,	0,0,0,0,	1,0);

		// 前平面
		Vertex v5(-1,1,-1,1,	0,0,-1,	255,0,0,0,	0,0);  
		Vertex v6(-1,-1,-1,1,	0,0,-1,	0,255,0,0,	1,0);
		Vertex v7(1,-1,-1,1,	0,0,-1,	0,0,255,0,	1,1);
		Vertex v8(1,1,-1,1,	0,0,-1,	0,0,0,0,	1,0);


		// 右平面
		Vertex v9 (1,1,-1,1,	1,0,0,	255,0,0,0,	0,0);  
		Vertex v10(1,-1,-1,1,	1,0,0,	0,255,0,0,	1,0);
		Vertex v11(1,-1,1,1,	1,0,0,	0,0,255,0,	1,1);
		Vertex v12(1,1,1,1,	1,0,0,	0,0,0,0,	1,0);


		// 下平面
		Vertex v13(-1,-1,-1,1,	0,-1,0,	255,0,0,0,	0,0);  
		Vertex v14(-1,-1,1,1,	0,-1,0,	0,255,0,0,	1,0);
		Vertex v15(1,-1,1,1,	0,-1,0,	0,0,255,0,	1,1);
		Vertex v16(1,-1,-1,1,	0,-1,0,	0,0,0,0,	1,0);

		// 后平面
		Vertex v17(1,1,1,1,	0,0,1,	255,0,0,0,	0,0);  
		Vertex v18(1,-1,1,1,	0,0,1,	0,255,0,0,	1,0);
		Vertex v19(-1,-1,1,1,	0,0,1,	0,0,255,0,	1,1);
		Vertex v20(-1,1,1,1,	0,0,1,	0,0,0,0,	1,0);


		// 左平面
		Vertex v21(-1,1,1,1,	-1,0,0,	255,0,0,0,	0,0);  
		Vertex v22(-1,-1,1,1,	-1,0,0,	0,255,0,0,	1,0);
		Vertex v23(-1,-1,-1,1,	-1,0,0,	0,0,255,0,	1,1);
		Vertex v24(-1,1,-1,1,	-1,0,0,	0,0,0,0,	1,0);

		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		vertexs.push_back(v4);

		vertexs.push_back(v5);
		vertexs.push_back(v6);
		vertexs.push_back(v7);
		vertexs.push_back(v8);

		vertexs.push_back(v9);
		vertexs.push_back(v10);
		vertexs.push_back(v11);
		vertexs.push_back(v12);

		vertexs.push_back(v13);
		vertexs.push_back(v14);
		vertexs.push_back(v15);
		vertexs.push_back(v16);

		vertexs.push_back(v17);
		vertexs.push_back(v18);
		vertexs.push_back(v19);
		vertexs.push_back(v20);

		vertexs.push_back(v21);
		vertexs.push_back(v22);
		vertexs.push_back(v23);
		vertexs.push_back(v24);


		indexs = { 0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};

		CalcWrodlMatrix();

	}

	Cube(Vector3 _origin, float _long, float _width, float _height):
		origin(_origin),longer(_long),width(_width),height(height)
	{

		// 上平面
		Vertex v1(-1, 1, 1, 1, 0, 1, 0, 255, 0, 0, 0, 0, 0);
		Vertex v2(-1, 1, -1, 1, 0, 1, 0, 0, 255, 0, 0, 1, 0);
		Vertex v3(1, 1, -1, 1, 0, 1, 0, 0, 0, 255, 0, 1, 1);
		Vertex v4(1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0);

		// 前平面
		Vertex v5(-1, 1, -1, 1, 0, 0, -1, 255, 0, 0, 0, 0, 0);
		Vertex v6(-1, -1, -1, 1, 0, 0, -1, 0, 255, 0, 0, 1, 0);
		Vertex v7(1, -1, -1, 1, 0, 0, -1, 0, 0, 255, 0, 1, 1);
		Vertex v8(1, 1, -1, 1, 0, 0, -1, 0, 0, 0, 0, 1, 0);


		// 右平面
		Vertex v9(1, 1, -1, 1, 1, 0, 0, 255, 0, 0, 0, 0, 0);
		Vertex v10(1, -1, -1, 1, 1, 0, 0, 0, 255, 0, 0, 1, 0);
		Vertex v11(1, -1, 1, 1, 1, 0, 0, 0, 0, 255, 0, 1, 1);
		Vertex v12(1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0);


		// 下平面
		Vertex v13(-1,-1,-1,1,	0,-1,0,	255,0,0,0,	0,0);  
		Vertex v14(-1,-1,1,1,	0,-1,0,	0,255,0,0,	1,0);
		Vertex v15(1,-1,1,1,	0,-1,0,	0,0,255,0,	1,1);
		Vertex v16(1,-1,-1,1,	0,-1,0,	0,0,0,0,	1,0);

		// 后平面
		Vertex v17(1,1,1,1,	0,0,1,	255,0,0,0,	0,0);  
		Vertex v18(1,-1,1,1,	0,0,1,	0,255,0,0,	1,0);
		Vertex v19(-1,-1,1,1,	0,0,1,	0,0,255,0,	1,1);
		Vertex v20(-1,1,1,1,	0,0,1,	0,0,0,0,	1,0);


		// 左平面
		Vertex v21(-1,1,1,1,	-1,0,0,	255,0,0,0,	0,0);  
		Vertex v22(-1,-1,1,1,	-1,0,0,	0,255,0,0,	1,0);
		Vertex v23(-1,-1,-1,1,	-1,0,0,	0,0,255,0,	1,1);
		Vertex v24(-1,1,-1,1,	-1,0,0,	0,0,0,0,	1,0);

		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		vertexs.push_back(v4);

		vertexs.push_back(v5);
		vertexs.push_back(v6);
		vertexs.push_back(v7);
		vertexs.push_back(v8);

		vertexs.push_back(v9);
		vertexs.push_back(v10);
		vertexs.push_back(v11);
		vertexs.push_back(v12);

		vertexs.push_back(v13);
		vertexs.push_back(v14);
		vertexs.push_back(v15);
		vertexs.push_back(v16);

		vertexs.push_back(v17);
		vertexs.push_back(v18);
		vertexs.push_back(v19);
		vertexs.push_back(v20);

		vertexs.push_back(v21);
		vertexs.push_back(v22);
		vertexs.push_back(v23);
		vertexs.push_back(v24);


		indexs = { 0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};


		CalcWrodlMatrix();
	}

private:
	void CalcWrodlMatrix()
	{
		worldMatrix = Matrix4x4(longer, 0, 0, longer * origin.x / 2,
			0, height, 0, height *  origin.y / 2,
			0, 0, width, width * origin.z / 2,
			0, 0, 0, 1);
	}
};




class Plane : public Geometry
{
private:
	Vector3 origin;
	float longer;
	float width;

public:
	Plane() :origin(0, -1.01f, 0), longer(4), width(4)
	{
		// 上平面
		Vertex v1(-1,0,1,1,	0,1,0,	255,0,0,0,	0,0);  
		Vertex v2(-1,0,-1,1,	0,1,0,	0,255,0,0,	1,0);
		Vertex v3(1,0,-1,1,	0,1,0,	0,0,255,0,	1,1);
		Vertex v4(1,0,1,1,	0,1,0,	0,0,0,0,	1,0);

		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		vertexs.push_back(v4);

		indexs = { 0,1,2,  0,2,3 };
		CalcWorldMatrix();
	}

	Plane(Vector3 _origin, float longer, float width) :
		origin(_origin), longer(longer), width(width)
	{
		Vertex v1(-1, 0, -1, 1, -1, 0, 1, 255, 255, 255, 0, 0, 0);
		Vertex v2(1, 0, -1, 1, 1, 0, -1, 255, 255, 255, 0, 0, 1);
		Vertex v3(1, 0, 1, 1, 1, 0, 1, 255, 255, 255, 0, 1, 0);
		Vertex v4(-1, 0, 1, 1, -1, 0, 1, 255, 255, 255, 0, 1, 1);

		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		vertexs.push_back(v4);

		indexs = { 0,1,2,  0,2,3 };
		CalcWorldMatrix();
	}

private:
	void CalcWorldMatrix()
	{
		worldMatrix = { longer, 0, 0, longer * origin.x / 2,
		0, 1, 0, origin.y,
		0, 0, width, width * origin.z / 2,
		0, 0, 0, 1 };
	}
};