#include "PMatrix.h"


namespace DX
{
	PMatrix PMatrix::ViewLookAt(Vector3& eye, Vector3& at, Vector3& up)
	{
		Vector3 vec_look = (at - eye).Normalize();
		float dot = vec_look.Dot(up);
		Vector3 vec_up = (up - (vec_look * dot)).Normalize();
		Vector3 vec_right = (vec_up.Cross(vec_look)).Normalize();

		_11 = vec_right.x; _12 = vec_up.x; _13 = vec_look.x;
		_21 = vec_right.y; _22 = vec_up.y; _23 = vec_look.y;
		_31 = vec_right.z; _32 = vec_up.z; _33 = vec_look.z;
		_41 = -(eye.Dot(vec_right));
		_42 = -(eye.Dot(vec_up));
		_43 = -(eye.Dot(vec_look));

		return *this;

	}

	PMatrix PMatrix::PerspectiveFovLH(float plane_near, float plane_far, float fov, float aspect)
	{
		
		PMatrix mat;
		ZeroMemory(&mat, sizeof(PMatrix));
		mat._11 = (1 / tanf(fov * 0.5f)) / aspect;
		mat._22 = 1 / tanf(fov * 0.5f);
		mat._33 = plane_far / (plane_far - plane_near);
		mat._43 = plane_far / (plane_far - plane_near) * -(plane_near);
		mat._34 = 1;
		*this = mat;

		return mat;
	}

	void PMatrix::Translation(const Vector3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}
	void PMatrix::Scale(const Vector3& v)
	{
		_11 = v.x;
		_22 = v.y;
		_33 = v.z;
	}
	void PMatrix::XRotate(float radian)
	{
		float cos = cosf(radian);
		float sin = sinf(radian);

		_22 = cos;		_23 = sin;
		_32 = -sin;		_33 = cos;

	}
	void PMatrix::YRotate(float radian)
	{
		float cos = (float)cosf(radian);
		float sin = (float)sinf(radian);

		_11 = cos;	_13 = -sin;
		_31 = sin;  _33 = cos;
	}
	void PMatrix::ZRotate(float radian)
	{
		float cos = (float)cosf(radian);
		float sin = (float)sinf(radian);
		_11 = cos;	_12 = sin;
		_21 = -sin; _22 = cos;
	}
	PMatrix PMatrix::Transpose()
	{
		PMatrix mat;
		mat._11 = _11; mat._12 = _21; mat._13 = _31; mat._14 = _41;
		mat._21 = _12; mat._22 = _22; mat._23 = _32; mat._24 = _42;
		mat._31 = _13; mat._32 = _23; mat._33 = _33; mat._34 = _43;
		mat._41 = _14; mat._42 = _24; mat._43 = _34; mat._44 = _44;

		return mat;
	}
};
