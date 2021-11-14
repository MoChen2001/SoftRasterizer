#pragma once
#include <math.h>


/// <summary>
///  数学辅助类
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
	///  弧度制到欧拉制
	/// </summary>
	static float PiToEuler(float piNumber)
	{
		return piNumber * PiDivByOne * 180;
	}

	/// <summary>
	///  欧拉制到弧度制
	/// </summary>
	static float EulerToPi(float angle)
	{
		return (angle / 180) * Pi;
	}
};

