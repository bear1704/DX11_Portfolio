#pragma once
#include "PShape.h"

const int kNumberofFrustumPlane = 6;
const int kNumberofFrustumVertex = 8;

//FRONT : 평면 전방(내부)   SPANNING : 평면에 교차  BACK : 평면 후방(외부) 
enum class RELATIVE_POSITION
{
	FRONT,
	SPANNING,
	BACK,
};

struct P_PLANE
{
	//ax + by + cz + d ::::  a,b,c => 평면노말 :: d = 원점과의 거리
	float a, b, c, d;  
	
	bool CreatePlane(
		D3DXVECTOR3 v0,
		D3DXVECTOR3 v1,
		D3DXVECTOR3 v2);
	bool CreatePlane(
		D3DXVECTOR3 normal,
		D3DXVECTOR3 v1);
	void Normalize();
};

class PFrustum
{
public:
	P_PLANE		frustum_plane_[kNumberofFrustumPlane];
	D3DXVECTOR3 frustum_vertex_[kNumberofFrustumVertex];
	PBoxObject	frustum_box_obj_;
	D3DXMATRIX	mat_view_;
	D3DXMATRIX	mat_proj_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;

public:
	void CreateFrustum(D3DXMATRIX& mat_view, D3DXMATRIX& mat_proj);
	bool CheckCollisionOBB(P_BOX& box);
	RELATIVE_POSITION CheckOBBRelativePos(P_BOX& box);

public:
	bool PreRender(ID3D11DeviceContext* context, int stride_length);
	bool PostRender(ID3D11DeviceContext* context);
	bool Render(ID3D11DeviceContext* context);

};

