#pragma once



#include "VectorX.h"
#include "MathHelper.h"


/// <summary>
///  摄像机类
/// </summary>
class Camera
{
public:
	Camera() {SetLens(0.25f * MathHelper::Pi, 1.0f, 1.0f, 1000.0f);};

	~Camera() {};
	Camera(const Camera& rhs) = delete;
	Camera operator=(const Camera& rhs) = delete;


public:

	void SetLens(float fovY, float aspect, float zn, float zf);
	void SetPosition(float x, float y, float z);
	void SetPosition(Vector3 pos);
	void SetRight(Vector3 right);
	void SetUp(Vector3 up);
	void SetTarget(Vector3 target);



	Vector3 GetPosition();
	Matrix4x4 GetMyView();
	Matrix4x4 GetMyProj();



private:
	Vector3 mPosition = { 0.0f, 0.0f, 0.0f };
	Vector3 mRight = { 1.0f, 0.0f, 0.0f };
	Vector3 mUp = { 0.0f, 1.0f, 0.0f };
	Vector3 mTarget = { 0.0f, 0.0f, 0.0f };


	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;

	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;


	Matrix4x4 mView;
	Matrix4x4 mProj;


	// 用来判定是否摄像机被更新
	bool mViewDirty = false;
};

