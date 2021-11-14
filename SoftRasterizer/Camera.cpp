#include "Camera.h"

/// <summary>
/// ���ø������,������һ����ʼ��������ĺ�������Щ��������������֮��Ͳ����ٱ�����
/// </summary>
/// <param name="fovY">��ֱ�ӳ���</param>
/// <param name="aspect">��߱�</param>
/// <param name="zn">�������</param>
/// <param name="zf">��Զ����</param>
void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;


	mNearWindowHeight =  2.0f * mNearZ * std::tanf( mFovY * 0.5f);
	mFarWindowHeight = 2.0f * mFarZ * std::tanf(mFovY * 0.5f);

	// ͶӰ���󣬻��Ǳ������ⶫ��...
	Matrix4x4 proj = { std::atan(fovY) * aspect / 2, 0, 0, 0,
		0, std::atan(fovY) / 2, 0, 0,
		0, 0, -1 * (mNearZ + mFarZ) / (mFarZ - mNearZ), -1 * (2 * mNearZ * mFarZ) / (mFarZ - mNearZ),
		0, 0, -1, 0 };

	mProj = proj;

}


void Camera::SetPosition(float x, float y, float z)
{
	Vector3 pos(x, y, z);
	mPosition = pos;
	mViewDirty = true;
}

void Camera::SetPosition(Vector3 pos)
{
	mPosition = pos;
	mViewDirty = true;
}

void Camera::SetRight(Vector3 right)
{
	mRight = right;
	mViewDirty = true;
}


void Camera::SetTarget(Vector3 target)
{
	mTarget = target;
	mViewDirty = true;
}

void Camera::SetUp(Vector3 up)
{
	mUp = up;
	mViewDirty = true;
}


Vector3& Camera::GetPosition()
{
	Vector3 res = mPosition;
	return res;
}


Matrix4x4& Camera::GetMyProj()
{
	Matrix4x4 res = mProj;
	return res;
}


Matrix4x4& Camera::GetMyView()
{
	if (mViewDirty)
	{
		//Matrix4x4 mTV = { mTarget.x, mUp.x, mRight.x, -1 * mPosition.x,
		//	mTarget.y, mUp.y, mRight.y, -1 * mPosition.y,
		//	mTarget.z, mUp.z, mRight.z, -1 * mPosition.z,
		//	0, 0, 0, 1};

		Matrix4x4 mTV = { 1,0,0, -1 * mPosition.x,
			0, 1, 0 , -1 * mPosition.y,
			0, 0, 1 , -1 * mPosition.z,
			0, 0, 0, 1};

		mView = mTV;
	}
	Matrix4x4 res = mView;
	return res;
}



