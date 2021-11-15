#pragma once
#include <iostream>
/// <summary>
///  主要是存储各种向量和矩阵，二维，三维，四维向量以及运算
///  向量默认为列向量
/// </summary>




#pragma region 二维向量，主要是 UV 啥的
class Vector2
{
public:
	float u,xA;


/// <summary>
///  主要是重载的内部的函数
/// </summary>
public:
	Vector2():u(0),xA(0) {}
	Vector2(float _u, float _v) :
		u(_u), xA(_v) {}

	Vector2(const Vector2& rhs) 
	{
		this->u = rhs.u;
		this->xA = rhs.xA;
	}
	Vector2 operator=(const Vector2& rhs)
	{
		this->u = rhs.u;
		this->xA = rhs.xA;
		return *this;
	}

	Vector2 operator*(int s)  const
	{
		Vector2 res(this->u, this->xA);
		res.u *= s;
		res.xA *= s;
		return res;
	}
	Vector2& operator*(float s) const
	{
		Vector2 res(this->u, this->xA);
		res.u *= s;
		res.xA *= s;
		return res;
	}
	Vector2 operator+(const Vector2& rhs) const
	{
		Vector2 res(this->u, this->xA);
		res.u += rhs.u;
		res.xA += rhs.xA;
		return res;
	}

	Vector2 operator-(const Vector2& rhs) const
	{
		Vector2 res(this->u, this->xA);
		res.u -= rhs.u;
		res.xA -= rhs.xA;
		return res;
	}

};

#pragma endregion


#pragma region 三维向量 ,最常用的一个
class Vector3
{
public:
	float x,y,z;


/// <summary>
///  主要是重载的内部的函数
/// </summary>
public:
	Vector3():x(0),y(0),z(0) {}
	Vector3(float _x, float _y,float _z) :
		x(_x), y(_y),z(_z) {}

	Vector3(const Vector3& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
	}
	Vector3 operator=(const Vector3& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		return *this;
	}
	Vector3 operator*(int s) const
	{
		Vector3 res(this->x, this->y, this->z);
		res.x *= s;
		res.y *= s;
		res.z *= s;
		return res;
	}
	Vector3 operator*(float s) const
	{
		Vector3 res(this->x, this->y, this->z);
		res.x *= s;
		res.y *= s;
		res.z *= s;
		return res;
	}
	Vector3 operator+(const Vector3& rhs) const
	{
		Vector3 res(this->x, this->y, this->z);
		res.x += rhs.x;
		res.y += rhs.y;
		res.z += rhs.z;
		return res;
	}

	Vector3 operator-(const Vector3& rhs) const
	{
		Vector3 res(this->x, this->y, this->z);
		res.x -= rhs.x;
		res.y -= rhs.y;
		res.z -= rhs.z;
		return res;
	}

public:

	float Distance() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}


	Vector3 Normalize() const
	{
		Vector3 res;
		float d = Distance();
		res.x = x / d;
		res.y = y / d;
		res.z = z / d;
		return res;
	}


	float Dot(const Vector3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}


	Vector3 Cross(const Vector3& rhs) const
	{
		Vector3 res;
		res.x = y * rhs.z - z * rhs.y;
		res.y = z * rhs.x - x * rhs.z;
		res.z = x * rhs.y - y * rhs.x;

		return res;
	}

};


#pragma endregion


#pragma region 四维向量,主要用作颜色啥的
class Vector4
{
public:
	float x, y, z, w;

/// <summary>
///  主要是重载的内部的函数
/// </summary>
public:
	Vector4() :x(0), y(0), z(0), w(0) {}
	Vector4(float _x, float _y, float _z, float _w) :
		x(_x), y(_y), z(_z), w(_w) {}
	Vector4(const Vector4& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		this->w = rhs.w;
	};
	Vector4 operator=(const Vector4& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		this->w = rhs.w;
		return *this;
	}


	Vector4 operator*(int s) const
	{
		Vector4 res(this->x, this->y, this->z, this->w);
		res.x *= s;
		res.y *= s;
		res.z *= s;
		res.w *= s;
		return res;
	}
	Vector4 operator*(float s) const
	{
		Vector4 res(this->x, this->y, this->z, this->w);
		res.x *= s;
		res.y *= s;
		res.z *= s;
		res.w *= s;
		return res;
	}
	Vector4 operator+(const Vector4& rhs)
	{
		Vector4 res(this->x, this->y, this->z, this->w);
		res.x += rhs.x;
		res.y += rhs.y;
		res.z += rhs.z;
		res.w += rhs.w;
		return res;
	}
	Vector4 operator-(const Vector4& rhs)
	{
		Vector4 res(this->x, this->y, this->z, this->w);
		res.x -= rhs.x;
		res.y -= rhs.y;
		res.z -= rhs.z;
		res.w -= rhs.w;
		return res;
	}


public:

	int GetIntColor()
	{
		int _x = (int)x;
		int _y = (int)y;
		int _z = (int)z;
		return ((_x << 16) | (_y << 8) | _z);
	}

};


#pragma endregion



#pragma region 二维矩阵
/// <summary>
/// 二维方阵，结构为：
/// x11  x12
/// x21  x22
/// </summary>
class Matrix2x2
{
public:
	float x11, x12;
	float x21, x22;

public:
	/// <summary>
	///  默认为单位矩阵
	/// </summary>
	Matrix2x2() :x11(1),x12(0),x21(0),x22(1){}
	Matrix2x2(float _x11, float _x12, float _x21, float _x22) :
		x11(_x11), x12(_x12), x21(_x21), x22(_x22) {}

	Matrix2x2(const Matrix2x2& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x21 = rhs.x21;
		x22 = rhs.x22;
	}

	Matrix2x2 operator=(const Matrix2x2& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x21 = rhs.x21;
		x22 = rhs.x22;
		return *this;
	}

	Matrix2x2 operator*(const Matrix2x2& rhs) const
	{
		Matrix2x2 res;
		res.x11 = x11 * rhs.x11 + x12 * rhs.x21;
		res.x12 = x11 * rhs.x12 + x12 * rhs.x22;
		res.x21 = x21 * rhs.x11 + x22 * rhs.x21;
		res.x22 = x21 * rhs.x12 + x22 * rhs.x22;
		return res;
	}

	Vector2 operator*(const Vector2& rhs) const
	{
		Vector2 res;
		res.u = x11 * rhs.u + x12 * rhs.xA;
		res.xA = x21 * rhs.u + x22 * rhs.xA;

		return res;
	}

	Matrix2x2 operator*(const float s) const
	{
		Matrix2x2 res(*this);
		res.x11 *= s;
		res.x12 *= s;
		res.x21 *= s;
		res.x22 *= s;
		return res;
	}

	Matrix2x2 operator+(const Matrix2x2& rhs) const
	{
		Matrix2x2 res;

		res.x11 = x11 + rhs.x11;
		res.x12 = x12 + rhs.x12;
		res.x21 = x21 + rhs.x21;
		res.x22 = x22 + rhs.x11;

		return res;
	}

public:
	float Determinant() const
	{
		return x11 * x22 - x12 * x21;
	}

	Matrix2x2 Transpose() const
	{
		Matrix2x2 res(x11, x21, x12, x22);
		return res;
	}

	/// <summary>
	///  分项相乘
	/// </summary>
	Matrix2x2 Mul(const Matrix2x2& rhs) const
	{
		Matrix2x2 res;
		res.x11 = x11 * rhs.x11;
		res.x12 = x12 * rhs.x12;
		res.x21 = x21 * rhs.x21;
		res.x11 = x22 * rhs.x22;
		return res;
	}

	Matrix2x2 Inverse() const
	{
		Matrix2x2 res;
		float deter = Determinant();

		res.x11 = x22 / deter;
		res.x12 = -1 * x21 / deter;
		res.x21 = -1 * x12 / deter;
		res.x22 = x11 / deter;
		return res;
	}

};


#pragma endregion 


#pragma region 三维矩阵

class Matrix3x3
{
public:
	float x11, x12, x13;
	float x21, x22, x23;
	float x31, x32, x33;

public:
	Matrix3x3() :x11(1), x12(0), x13(0), x21(0), x22(1), x23(0), x31(0), x32(0), x33(1) {}
	Matrix3x3(float _x11, float _x12, float _x13, 
		float _x21, float _x22, float _x23, 
		float _x31, float _x32, float _x33) :
		x11(_x11), x12(_x12), x13(_x13), x21(_x21), x22(_x22), x23(_x23), x31(_x31), x32(_x32), x33(_x33) {}
	Matrix3x3(const Matrix3x3& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x13 = rhs.x13;
		x21 = rhs.x21;
		x22 = rhs.x22;
		x23 = rhs.x23;
		x31 = rhs.x31;
		x32 = rhs.x32;
		x33 = rhs.x33;
	}
	Matrix3x3 operator=(const Matrix3x3& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x13 = rhs.x13;
		x21 = rhs.x21;
		x22 = rhs.x22;
		x23 = rhs.x23;
		x31 = rhs.x31;
		x32 = rhs.x32;
		x33 = rhs.x33;
		return *this;
	}

	
	Matrix3x3 operator+(const Matrix3x3& rhs) const
	{
		Matrix3x3 res(*this);
		res.x11 += rhs.x11;
		res.x12 += rhs.x12;
		res.x13 += rhs.x13;
		res.x21 += rhs.x21;
		res.x22 += rhs.x22;
		res.x23 += rhs.x23;
		res.x31 += rhs.x31;
		res.x32 += rhs.x32;
		res.x33 += rhs.x33;
		return res;
	}

	Matrix3x3 operator*(const Matrix3x3& rhs) const
	{
		Matrix3x3 res;
		res.x11 += x11 * rhs.x11 + x12 * rhs.x21 + x13 * rhs.x31;
		res.x12 += x11 * rhs.x12 + x12 * rhs.x22 + x13 * rhs.x32;
		res.x13 += x11 * rhs.x13 + x12 * rhs.x23 + x13 * rhs.x33;

		res.x21 += x21 * rhs.x11 + x22 * rhs.x21 + x23 * rhs.x31;
		res.x22 += x21 * rhs.x12 + x22 * rhs.x22 + x23 * rhs.x32;
		res.x23 += x21 * rhs.x13 + x22 * rhs.x23 + x23 * rhs.x33;

		res.x31 += x31 * rhs.x11 + x32 * rhs.x21 + x33 * rhs.x31;
		res.x32 += x31 * rhs.x12 + x32 * rhs.x22 + x33 * rhs.x32;
		res.x33 += x31 * rhs.x13 + x32 * rhs.x23 + x33 * rhs.x33;

		return res;
	}

	Vector3 operator*(const Vector3& rhs) const
	{
		Vector3 res;
		res.x = x11 * rhs.x + x12 * rhs.y + x13 * rhs.z;
		res.y = x21 * rhs.x + x22 * rhs.y + x23 * rhs.z;
		res.z = x31 * rhs.x + x32 * rhs.y + x33 * rhs.z;
		return res;
	}

	Matrix3x3 operator*(const float s) const
	{
		Matrix3x3 res(*this);
		res.x11 *= s;
		res.x12 *= s;
		res.x13 *= s;

		res.x21 *= s;
		res.x22 *= s;
		res.x23 *= s;

		res.x31 *= s;
		res.x32 *= s;
		res.x33 *= s;
		return res;
	}

public:

	float Determinant() const
	{
		float res;
		res = x11 * x22 * x33
			+ x12 * x23 * x31
			+ x13 * x21 * x32
			- x13 * x22 * x31
			- x11 * x23 * x32
			- x12 * x21 * x33;

		return res;
	}

	Matrix3x3 Transpose() const
	{
		Matrix3x3 res(x11, x21, x31, 
			x12, x22, x32, 
			x13, x23, x33);
		return res;
	}

	Matrix3x3 inverse() const
	{
		float s = 1 / Determinant();
		Matrix3x3 res;

		// 代数余子式 ，按行求按列排
		res.x11 = x22 * x33 - x23 * x32;
		res.x21 = -1 * (x21 * x33 - x23 * x31);
		res.x31 = x21 * x32 - x22 * x31;
		res.x12 = -1 * (x12 * x33 - x13 * x31);
		res.x22 = x11 * x33 - x31 * x13;
		res.x32 = -1 * (x11 * x32 - x12 * x31);
		res.x13 = x12 * x23 - x13 * x22;
		res.x23 = -1 * (x11 * 32 - x12 * 31);
		res.x33 = x11 * x22 - x21 * x12;
		
		res = res * s;

		return res;
	}

	/// <summary>
	///  分项乘
	/// </summary>
	Matrix3x3 Mul(const Matrix3x3& rhs) const
	{
		Matrix3x3 res(*this);
		res.x11 *= rhs.x11;
		res.x12 *= rhs.x12;
		res.x13 *= rhs.x13;
		res.x21 *= rhs.x21;
		res.x22 *= rhs.x22;
		res.x23 *= rhs.x23;
		res.x31 *= rhs.x31;
		res.x32 *= rhs.x32;
		res.x33 *= rhs.x33;
		return res;
	}
};


#pragma endregion


class Matrix4x4
{
public:
	float x11, x12, x13, x14;
	float x21, x22, x23, x24;
	float x31, x32, x33, x34;
	float x41, x42, x43, x44;

public:
	Matrix4x4() :x11(1), x12(0), x13(0),x14(0), x21(0), x22(1), x23(0),x24(0),
		x31(0), x32(0), x33(1) ,x34(0),x41(0),x42(0),x43(0),x44(1){}
	Matrix4x4(float _x11, float _x12, float _x13, float _x14,
		float _x21, float _x22, float _x23, float _x24,
		float _x31, float _x32, float _x33,float _x34,
		float _x41, float _x42, float _x43, float _x44) :
		x11(_x11), x12(_x12), x13(_x13), x14(_x14),
		x21(_x21), x22(_x22), x23(_x23), x24(_x24),
		x31(_x31), x32(_x32), x33(_x33), x34(_x34),
		x41(_x41),x42(_x42),x43(_x43),x44(_x44){}

	Matrix4x4(const Matrix4x4& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x13 = rhs.x13;
		x14 = rhs.x14;

		x21 = rhs.x21;
		x22 = rhs.x22;
		x23 = rhs.x23;
		x24 = rhs.x24;

		x31 = rhs.x31;
		x32 = rhs.x32;
		x33 = rhs.x33;
		x34 = rhs.x34;

		x41 = rhs.x41;
		x42 = rhs.x42;
		x43 = rhs.x43;
		x44 = rhs.x44;

	}

	Matrix4x4 operator=(const Matrix4x4& rhs)
	{
		x11 = rhs.x11;
		x12 = rhs.x12;
		x13 = rhs.x13;
		x14 = rhs.x14;

		x21 = rhs.x21;
		x22 = rhs.x22;
		x23 = rhs.x23;
		x24 = rhs.x24;

		x31 = rhs.x31;
		x32 = rhs.x32;
		x33 = rhs.x33;
		x34 = rhs.x34;

		x41 = rhs.x41;
		x42 = rhs.x42;
		x43 = rhs.x43;
		x44 = rhs.x44;
		return *this;
	}


	Matrix4x4 operator+(const Matrix4x4& rhs) const
	{
		Matrix4x4 res(*this);
		res.x11 += rhs.x11;
		res.x12 += rhs.x12;
		res.x13 += rhs.x13;
		res.x14 += rhs.x14;

		res.x21 += rhs.x21;
		res.x22 += rhs.x22;
		res.x23 += rhs.x23;
		res.x24 += rhs.x24;

		res.x31 += rhs.x31;
		res.x32 += rhs.x32;
		res.x33 += rhs.x33;
		res.x34 += rhs.x34;

		res.x41 += rhs.x41;
		res.x42 += rhs.x42;
		res.x43 += rhs.x43;
		res.x44 += rhs.x44;

		return res;
	}

	Matrix4x4 operator*(const Matrix4x4& rhs) const
	{
		Matrix4x4 res;
		res.x11 += x11 * rhs.x11 + x12 * rhs.x21 + x13 * rhs.x31 + x14 * rhs.x41;
		res.x12 += x11 * rhs.x12 + x12 * rhs.x22 + x13 * rhs.x32 + x14 * rhs.x42;
		res.x13 += x11 * rhs.x13 + x12 * rhs.x23 + x13 * rhs.x33 + x14 * rhs.x43;
		res.x14 += x11 * rhs.x14 + x12 * rhs.x24 + x13 * rhs.x34 + x14 * rhs.x44;

		res.x21 += x21 * rhs.x11 + x22 * rhs.x21 + x23 * rhs.x31 + x24 * rhs.x41;
		res.x22 += x21 * rhs.x12 + x22 * rhs.x22 + x23 * rhs.x32 + x24 * rhs.x42;
		res.x23 += x21 * rhs.x13 + x22 * rhs.x23 + x23 * rhs.x33 + x24 * rhs.x43;
		res.x24 += x21 * rhs.x14 + x22 * rhs.x24 + x23 * rhs.x34 + x24 * rhs.x44;

		res.x31 += x31 * rhs.x11 + x32 * rhs.x21 + x33 * rhs.x31 + x34 * rhs.x41;
		res.x32 += x31 * rhs.x12 + x32 * rhs.x22 + x33 * rhs.x32 + x34 * rhs.x42;
		res.x33 += x31 * rhs.x13 + x32 * rhs.x23 + x33 * rhs.x33 + x34 * rhs.x43;
		res.x34 += x31 * rhs.x14 + x32 * rhs.x24 + x33 * rhs.x34 + x34 * rhs.x44;

		res.x41 += x41 * rhs.x11 + x42 * rhs.x21 + x43 * rhs.x31 + x44 * rhs.x41;
		res.x42 += x41 * rhs.x12 + x42 * rhs.x22 + x43 * rhs.x32 + x44 * rhs.x42;
		res.x43 += x41 * rhs.x13 + x42 * rhs.x23 + x43 * rhs.x33 + x44 * rhs.x43;
		res.x44 += x41 * rhs.x14 + x42 * rhs.x24 + x43 * rhs.x34 + x44 * rhs.x44;

		return res;
	}

	Vector4 operator*(const Vector4& rhs) const
	{
		Vector4 res;
		res.x = x11 * rhs.x + x12 * rhs.y + x13 * rhs.z + x14 * rhs.w;
		res.y = x21 * rhs.x + x22 * rhs.y + x23 * rhs.z + x24 * rhs.w;
		res.z = x31 * rhs.x + x32 * rhs.y + x33 * rhs.z + x34 * rhs.w;
		res.w = x41 * rhs.x + x42 * rhs.y + x43 * rhs.z + x44 * rhs.w;
		return res;
	}

	Vector3 operator*(const Vector3& temp) const
	{
		Vector4 rhs = Vector4(temp.x, temp.y, temp.z, 0.0);

		Vector3 res;
		res.x = x11 * rhs.x + x12 * rhs.y + x13 * rhs.z + x14 * rhs.w;
		res.y = x21 * rhs.x + x22 * rhs.y + x23 * rhs.z + x24 * rhs.w;
		res.z = x31 * rhs.x + x32 * rhs.y + x33 * rhs.z + x34 * rhs.w;
		return res;
	}


	Matrix4x4 operator*(const float s) const
	{
		Matrix4x4 res(*this);
		res.x11 *= s;
		res.x12 *= s;
		res.x13 *= s;
		res.x14 *= s;

		res.x21 *= s;
		res.x22 *= s;
		res.x23 *= s;
		res.x24 *= s;

		res.x31 *= s;
		res.x32 *= s;
		res.x33 *= s;
		res.x34 *= s;

		res.x41 *= s;
		res.x42 *= s;
		res.x43 *= s;
		res.x44 *= s;

		return res;
	}


public:


	Matrix4x4 Transpose() const
	{
		Matrix4x4 res(x11, x21, x31, x41, 
			x12, x22, x32, x42,
			x13, x23, x33, x43,
			x14, x24, x34, x44);
		return res;
	}

};