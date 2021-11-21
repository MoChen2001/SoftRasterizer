/// ��Դ�Ͳ�����

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
	Vector4 fresnel;   // ���淴�����
	Vector4 diffuse;   // ���������
	float gloss;  // �⻬��

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