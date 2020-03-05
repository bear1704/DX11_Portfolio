#pragma once
#include "PVector.h"

namespace DX
{

	struct float4x4
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float mat[4][4];
		};

	};


	class PMatrix : public float4x4
	{
	public:
		PMatrix ViewLookAt(Vector3& eye, Vector3& at, Vector3& up);
		PMatrix PerspectiveFovLH(float plane_near, float plane_far, float fov, float aspect);

		void Identity()
		{
			_11 = _12 = _13 = _14 = 0.0f;
			_21 = _22 = _23 = _24 = 0.0f;
			_31 = _32 = _33 = _34 = 0.0f;
			_41 = _42 = _43 = _44 = 0.0f;
			_11 = _22 = _33 = _44 = 1.0f;

		}
		

		void Translation(const Vector3& v);
		void Scale(const Vector3& v);
		void XRotate(float radian);
		void YRotate(float radian);
		void ZRotate(float radian);
		PMatrix Transpose();



	};

}