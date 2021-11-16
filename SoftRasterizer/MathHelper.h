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


	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}



	template<typename T>
	static T Min(const T& x, const T& y)
	{
		return  x > y ? y : x;
	}

	template<typename T>
	static T Max(const T& x, const T& y)
	{
		return  x > y ? x : y;
	}
};

