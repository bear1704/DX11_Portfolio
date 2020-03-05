#include "PNormalMapHelper.h"

PNormalMapHelper::PNormalMapHelper()
{
}

PNormalMapHelper::~PNormalMapHelper()
{
}

void PNormalMapHelper::CreateTangentSpaceVectors
(D3DXVECTOR3* v0, D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR2 uv0, D3DXVECTOR2 uv1, D3DXVECTOR2 uv2, OUT_ D3DXVECTOR3* Tangent, OUT_ D3DXVECTOR3* BiNormal, OUT_ D3DXVECTOR3* vec_normal)
{
	D3DXVECTOR3 edge1 = *v1 - *v0;
	D3DXVECTOR3 edge2 = *v2 - *v0;
	D3DXVec3Normalize(&edge1, &edge1);
	D3DXVec3Normalize(&edge2, &edge2);
	//uv delta
	D3DXVECTOR2 delta_uv1 = uv1 - uv0;
	D3DXVECTOR2 delta_uv2 = uv2 - uv0;
	D3DXVec2Normalize(&delta_uv1, &delta_uv1);
	D3DXVec2Normalize(&delta_uv2, &delta_uv2);

	D3DXVECTOR3 binormal;
	float determinant = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * 
		delta_uv2.x);

	if (std::fabsf(determinant) < 1e-6f)
	{
		*Tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		binormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		*Tangent = (edge1 * delta_uv2.y - edge2 * delta_uv1.y) * determinant;
		binormal = (edge2 * delta_uv1.x - edge1 * delta_uv2.x);
	}

	D3DXVec3Normalize(Tangent, Tangent);
	D3DXVec3Normalize(&binormal, &binormal);

	D3DXVec3Cross(BiNormal, vec_normal, Tangent);
	float cross_inv = (D3DXVec3Dot(BiNormal, &binormal) < 0.0f) ? -1.0f : 1.0f;
	*BiNormal *= cross_inv;
}

void PNormalMapHelper::CreateTangentSpaceVectors
(D3DXVECTOR3* v1, D3DXVECTOR3* v2, D3DXVECTOR3* v3,
	float v1u, float v1v, float v2u, float v2v, float v3u, float v3v, 
	OUT_ D3DXVECTOR3* Tangent, OUT_ D3DXVECTOR3* BiNormal, OUT_ D3DXVECTOR3* vec_normal)
{
	D3DXVECTOR3 edge1 = *v2 - *v1;
	D3DXVECTOR3 edge2 = *v3 - *v1;

	float edge1_u = v2u - v1u;
	float edge1_v = v2u - v1u;
	float edge2_u = v3u - v1u;
	float edge2_v = v3u - v1u;

	float denominator = edge1_u * edge2_v - edge2_u * edge1_v;

	if (denominator < 0.0001f && denominator > -0.0001f)
	{
		*Tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		*BiNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		*vec_normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		float scale1 = 1.0f / denominator;
		
		D3DXVECTOR3 T;
		D3DXVECTOR3 B;
		D3DXVECTOR3 N;


		T = D3DXVECTOR3((edge2_v * edge1.x - edge1_v * edge2.x) * scale1,
			(edge2_v * edge1.y - edge1_v * edge2.y) * scale1,
			(edge2_v * edge1.z - edge1_v * edge2.z) * scale1);

		B = D3DXVECTOR3((-edge2_u * edge1.x + edge1_u * edge2.x) * scale1,
			(-edge2_u * edge1.y + edge1_u * edge2.y) * scale1,
			(-edge2_u * edge1.z + edge1_u * edge2.z) * scale1);

		D3DXVec3Cross(&N, &T, &B);

		float scale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) +
			(B.x * N.y * T.z - B.z * N.y * T.x) +
			(N.x * T.y * B.z - N.z * T.y * B.x));  //행렬판별식

		D3DXVECTOR3 vTemp;

		(*Tangent).x = D3DXVec3Cross(&vTemp, &B, &N)->x * scale2;
		(*Tangent).y = -(D3DXVec3Cross(&vTemp, &N, &T)->x * scale2);
		(*Tangent).z = D3DXVec3Cross(&vTemp, &T, &B)->x * scale2;
		D3DXVec3Normalize(&(*Tangent), &(*Tangent));

		(*BiNormal).x = -(D3DXVec3Cross(&vTemp, &B, &N)->y * scale2);
		(*BiNormal).y = D3DXVec3Cross(&vTemp, &N, &T)->y * scale2;
		(*BiNormal).z = -(D3DXVec3Cross(&vTemp, &T, &B)->y * scale2);
		D3DXVec3Normalize(&(*BiNormal), &(*BiNormal));

		(*vec_normal).x = D3DXVec3Cross(&vTemp, &B, &N)->z * scale2;
		(*vec_normal).y = -(D3DXVec3Cross(&vTemp, &N, &T)->z * scale2);
		(*vec_normal).z = D3DXVec3Cross(&vTemp, &T, &B)->z * scale2;
		D3DXVec3Normalize(&(*vec_normal), &(*vec_normal));
		//행렬을 수반행렬(Adjoint)로 만든 뒤. 행렬판별식으로 나누어
		//역행렬을 구하는 과정. tbasis2 p.175
	}



}
