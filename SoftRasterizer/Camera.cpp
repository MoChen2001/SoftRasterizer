#include "Camera.h"

/// <summary>
/// 设置各项参数,更像是一个初始化摄像机的函数，这些参数基本上设置之后就不会再被更改
/// </summary>
/// <param name="fovY">竖直视场角</param>
/// <param name="aspect">宽高比</param>
/// <param name="zn">最近距离</param>
/// <param name="zf">最远距离</param>
void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;


	Matrix4x4 proj = {  (1 / std::tan(fovY * 0.5f)) / aspect, 0, 0, 0,
		0, (1 / std::tan(fovY * 0.5f)), 0, 0,
		0, 0,  -1 * (mNearZ + mFarZ) / (mFarZ - mNearZ), -1 * (2 * mNearZ * mFarZ) / (mFarZ - mNearZ),
		0, 0, 1, 0 };

	mProj = proj;

}


void Camera::SetPosition(float x, float y, float z)
{
	mPosition = Vector3(x,y,z);
	mViewDirty = true;
}

void Camera::SetPosition(Vector3 pos)
{
	mPosition = pos;
	mViewDirty = true;
}



void Camera::SetTarget(Vector3 target)
{
	mTarget = target;
	mViewDirty = true;
}


Vector3& Camera::GetPosition()
{
	return mPosition;
}


Matrix4x4& Camera::GetMyProj()
{
	return mProj;
}


Matrix4x4& Camera::GetMyView()
{
	if (mViewDirty)
	{
		mUp = Vector3(0, 1, 0);
		mRight = mTarget.Cross(mUp); // x

		mUp = mRight.Cross(mTarget); // y

		Vector3 invPos = mPosition;

		Matrix4x4 _mView = { mRight.x, mRight.y, mRight.z, invPos.Dot(mRight) * -1,
			mUp.x, mUp.y, mUp.z, invPos.Dot(mUp) * -1,
			mTarget.x, mTarget.y, mTarget.z, invPos.Dot(mTarget) * -1,
			0, 0, 0, 1 };

		mView = _mView;

	}
	return mView;
}



