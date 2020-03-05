#pragma once
#include "PShape.h"

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

