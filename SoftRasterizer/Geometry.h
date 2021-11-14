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
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;

public:
	
	Matrix4x4 worldMatrix;
	std::vector<Vertex> vertexs;
	std::vector<int> indexs;
};



class Cube : Geometry
{
private:
	Vector3 origin;
	float longer;
	float width;
	float height;


};
