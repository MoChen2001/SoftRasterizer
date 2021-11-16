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
		pos(px,py,pz,pw),normal(nx,ny,nz), color(cx,cy,cz,cw),
	texcoord(tx,ty){}


	~Vertex() {}


public:
	Vector2 texcoord;
	Vector4 pos;
	Vector3 normal;
	Vector3 targent;
	Vector4 color;
};




/// <summary>
/// 几何类, 总的抽象类
/// </summary>
class Geometry
{
public:
	Geometry()  {}
	~Geometry() {}
	Geometry(const Geometry& rhs) = delete;
	Geometry& operator=(const Geometry& rhs) = delete;
	
public:
	
	Matrix4x4 worldMatrix;
	std::vector<Vertex> vertexs;
	std::vector<int> indexs;
};



class Cube : public Geometry
{
private:
	Vector3 origin;
	float longer;
	float width;
	float height;

public:
	Cube() :origin(10, 0, 0), longer(2), width(2), height(2) 
	{
		int halfLong = longer / 2;
		int halfwidth = width / 2;
		int halfHeight = height / 2;

		Vertex v1(-1, -1, -1, 1, 0, 0, 0, 255, 0, 0, 0, 0, 0);  
		Vertex v2(1, -1, -1, 1, 0, 0, 0, 0, 255, 0, 0, 0, 1);
		Vertex v3(1, -1, 1, 1, 0, 0, 0, 0, 0, 255, 0, 1, 0);
		Vertex v4(1, -1, 1, 1, 0, 0, 0, 150, 150, 150, 0, 1, 1);

		Vertex v5(-1, 1, -1, 1, 0, 0, 0, 255, 0, 0, 0, 0, 0);
		Vertex v6(1, 1, -1, 1, 0, 0, 0, 0, 255, 0, 0, 0, 1);
		Vertex v7(1, 1, 1, 1, 0, 0, 0, 0, 0, 255, 0, 1, 0);
		Vertex v8(-1, 1, 1, 1, 0, 0, 0, 150, 150, 150, 0, 1, 1);


		vertexs.push_back(v1);
		vertexs.push_back(v2);
		vertexs.push_back(v3);
		vertexs.push_back(v4);
		vertexs.push_back(v5);
		vertexs.push_back(v6);
		vertexs.push_back(v7);
		vertexs.push_back(v8);


		indexs = { 0,1,2,  // 下
		0,2,3,

		4,7,3, // 左
		4,3,0,

		4,5,6, // 上
		4,6,7,

		4,5,1, // 后
		4,1,0,

		7,6,2, // 前
		7,2,3,

		5,6,2,
		5,2,1
		};

		worldMatrix = Matrix4x4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 10,
			0, 0, 0, 1);

	}


};
