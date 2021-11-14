#pragma once
#include <math.h>


/// <summary>
///  ��ѧ������
/// </summary>
class MathHelper
{

public:
	static const float Pi;
	static const float PiDivTwo;
	static const float PiMulTwo;
	static const float PiDivFour;
	static const float PiDivByOne;

	/// <summary>
	///  �����Ƶ�ŷ����
	/// </summary>
	static float PiToEuler(float piNumber)
	{
		return piNumber * PiDivByOne * 180;
	}

	/// <summary>
	///  ŷ���Ƶ�������
	/// </summary>
	static float EulerToPi(float angle)
	{
		return (angle / 180) * Pi;
	}
};

