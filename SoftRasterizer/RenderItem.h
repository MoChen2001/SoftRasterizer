#pragma once
#include "Geometry.h"
#include <unordered_map>


class RenderItem
{

public:
	// DX 上用的也是智能指针，确实比较好用

	std::unordered_map<std::string, std::unique_ptr<Geometry>> mGeos;	

};

