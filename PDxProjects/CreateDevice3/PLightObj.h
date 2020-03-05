#pragma once
#include <D3DX11.h>
#include <D3DX10math.h>
#include <wrl.h>
#include "PDxHelper.h"
#include "PCamera.h"

struct LIGHT_CONSTANT_BUFFER
{
	D3DXMATRIX g_matInvWorld;

	D3DXVECTOR4 g_AmbientMaterial;
	D3DXVECTOR4 g_DiffuseMaterial;
	D3DXVECTOR4 g_SpecularMaterial;
	D3DXVECTOR4 g_EmissionMaterial;

	//Light
	D3DXVECTOR4			g_AmbientColor;
	D3DXVECTOR4			g_DiffuseColor;
	D3DXVECTOR4			g_SpecularColor;
	D3DXVECTOR4			g_vLightDir; // w = light damping(attenuation) : °¨¼è
	D3DXVECTOR4			g_vLightPos; // w = light radius
	D3DXVECTOR4			g_vEyeDir;// w = light intensity : °­µµ
	D3DXVECTOR4			g_vEyePos;// w = light radius	

};

struct LightTRS
{
	D3DXVECTOR3 light_trans_;
	D3DXVECTOR3 light_rot_;
	D3DXVECTOR3 light_scale_;
};

class PLightObj
{
private:
	ID3D11Device* device_;
	ID3D11DeviceContext* context_;
	LIGHT_CONSTANT_BUFFER cb_light_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_light_;
	D3DXVECTOR3 light_position_;
	D3DXVECTOR3 light_direction_;

	D3DXMATRIX light_world_;
	D3DXMATRIX light_init_world_;
	D3DXVECTOR3 light_init_pos_;
	LightTRS light_trs_;
	PCamera* camera_;

public:
	PLightObj();
	~PLightObj();

public:
	bool Init(D3DXVECTOR4 ambient_material, D3DXVECTOR4 ambient_color, D3DXVECTOR4 diffuse_material,
		D3DXVECTOR4 diffuse_color, D3DXVECTOR4 specular_material, D3DXVECTOR4 specular_color
		, ID3D11Device* device,ID3D11DeviceContext* context ,PCamera* camera);
	bool Frame();
	bool Render();
	bool Release();
	bool Update();  // updateresources

public:
	D3DXVECTOR3 light_direction();
	D3DXVECTOR3 light_position();


};

