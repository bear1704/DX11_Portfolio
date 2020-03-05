#pragma once
#include <windows.h>
#include <assert.h>
#include <cmath>

namespace DX
{

	const float kFloatEpsilon = static_cast<float>(0.001f);
	const float kPI = static_cast<float>(3.141592654f);

#define DEGREE_TO_RADIAN(degree) (degree*(kPI/180.0f))
#define RADIAN_TO_DEGREE(radian) (radian*(180.0f/kPI))

	struct float2
	{
		union
		{
			struct { float x, y; };
			float v[2];
		};
	};

	struct float3
	{
		union
		{
			struct { float x, y, z; };
			float v[3];

		};
	};

	struct float4
	{
		union
		{
			struct { float x, y, z, w; };
			float v[4];
		};
	};

	class Vector2 : public float2
	{
	public:
		Vector2();
		Vector2(const Vector2& v);
		Vector2(float fx, float fy);
		~Vector2();

		bool operator==(Vector2 const& v);
	};
	class Vector3 : public float3
	{
	public:
		Vector3();
		Vector3(const Vector3& v);
		Vector3(float fx, float fy, float fz);
		~Vector3();
	
	public:
		Vector3 operator+ (Vector3 const& v);
		Vector3 operator- (Vector3 const& v);
		Vector3 operator* (float const& scala);
		Vector3 operator/ (float const& scala);
		float Dot(const Vector3& v);
		Vector3 Cross(const Vector3& v);
		float Length();
		Vector3 Normalize();
		float Angle(Vector3& v);
		bool operator==(Vector3 const& v);

	};
	

	class Vector4 : public float4
	{
	public:
		Vector4();
		Vector4(const Vector4& v);
		Vector4(float fx, float fy, float fz, float fw);
		~Vector4();
		bool operator== (Vector4 const& v);

	};


}
