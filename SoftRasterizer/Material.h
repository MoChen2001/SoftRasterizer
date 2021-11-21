/// 光源和材质类

#pragma once
#include <iostream>
#include "VectorX.h"


class Material
{
public:
	Material() : gloss(0.1f) {}
	~Material() {}
	Material(const Material& rhs)
	{
		fresnel = rhs.fresnel;
		diffuse = rhs.diffuse;
		gloss = rhs.gloss;
	}
	Material& operator=(const Material& rhs)
	{
		fresnel = rhs.fresnel;
		diffuse = rhs.diffuse;
		gloss = rhs.gloss;
		return *this;
	}

public:
	std::string name;
	Vector4 fresnel;   // 镜面反射参数
	Vector4 diffuse;   // 漫反射参数
	float gloss;  // 光滑度

};


class Light
{
public:
	Light() {};
	Light(Vector3 o,Vector4 d,Vector4 c):
	dir(o),intensity(c){};
	~Light() {};
	Light(const Light& rhs)
	{
		dir = rhs.dir;
		intensity = rhs.intensity;
	}
	Light& operaotr(const Light& rhs)
	{
		dir = rhs.dir;
		intensity = rhs.intensity;
		return *this;
	}

public:
	Vector3 dir;
	Vector4 intensity;
};