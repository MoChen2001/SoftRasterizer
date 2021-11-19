#pragma once
/// 主要就是读取纹理啥的
#include <iostream>
#include <vector>
#include "VectorX.h"


class Texture
{
public:
	Texture() :width(0), height(0) {}
	~Texture() {};
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

public:
	int width;
	int height;
	std::vector<std::vector<Vector4>> tex;

public:

	void ReadJpg(std::string path);

	Vector4 GetColor(float u, float v);
};

