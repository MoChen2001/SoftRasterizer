#pragma once
#include "Geometry.h"
#include <unordered_map>


class RenderItem
{

public:
	// DX ���õ�Ҳ������ָ�룬ȷʵ�ȽϺ���

	std::unordered_map<std::string, std::unique_ptr<Geometry>> mGeos;	

};

