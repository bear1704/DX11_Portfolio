#pragma once
#include "PShape.h"
#include "PLightObj.h"
//
//struct CB_VS_ChangesEveryFrame
//{
//	D3DXMATRIX mat_normal;
//	D3DXVECTOR3 light_pos;
//	float padding1;
//	D3DXVECTOR3 camera_pos;
//	float padding2;
//	D3DXVECTOR3 vec_look;
//	float padding3;
//};
//struct CB_VS_NearlyNotChange
//{
//	D3DXVECTOR4 cb_AmbientLightColor;
//	D3DXVECTOR4 cb_DiffuseLightColor;
//	D3DXVECTOR4 cb_SpecularLightColor;
//};


class PNormalMapHelper
{
public:
	PNormalMapHelper();
	~PNormalMapHelper();

public:
	void CreateTangentSpaceVectors(
		D3DXVECTOR3* v0,
		D3DXVECTOR3* v1,
		D3DXVECTOR3* v2,
		D3DXVECTOR2 uv0,
		D3DXVECTOR2 uv1,
		D3DXVECTOR2 uv2,
		OUT_ D3DXVECTOR3* Tangent,
		OUT_ D3DXVECTOR3* BiNormal,
		OUT_ D3DXVECTOR3* vec_normal
		);

	void CreateTangentSpaceVectors(
		D3DXVECTOR3* v1,
		D3DXVECTOR3* v2,
		D3DXVECTOR3* v3,
		float v1u, float v1v,
		float v2u, float v2v,
		float v3u, float v3v,
		OUT_ D3DXVECTOR3* Tangent,
		OUT_ D3DXVECTOR3* BiNormal,
		OUT_ D3DXVECTOR3* vec_normal);
	

};

