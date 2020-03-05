#pragma once
#include <D3DX11.h>
#include <D3DX10math.h>
#include <wrl.h>
#include "PDxHelper.h"
#include "PCamera.h"

struct CB_VS_ChangesEveryFrame
{
	D3DXMATRIX mat_normal;
	D3DXVECTOR3 vec_light;
	FLOAT padding1;
	D3DXVECTOR3 camera_pos;
	FLOAT padding2;
	D3DXVECTOR3 vec_look;
	FLOAT padding3;
};
struct CB_VS_LightNearlyNotChange
{
	D3DXVECTOR4 ambient_material;
	D3DXVECTOR4 diffuse_material;
	D3DXVECTOR4 specular_material;

	D3DXVECTOR4 ambient_color;
	D3DXVECTOR4 diffuse_color;
	D3DXVECTOR4 specular_color; 
	
	//구조체 값이 변경되면 반드시 true로 만들어야 함
	bool is_dirty;
	FLOAT specualr_intensity;
	D3DXVECTOR2 padding;
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
	CB_VS_LightNearlyNotChange cb_nearly_not_change_;
	CB_VS_ChangesEveryFrame cb_change_everyframe_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> cbuffer_light_nearly_not_changed_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cbuffer_change_every_frame_;

	D3DXVECTOR3 light_position_;
	D3DXVECTOR3 light_direction_;

	D3DXMATRIX light_world_;
	D3DXMATRIX light_init_world_;
	D3DXVECTOR3 light_init_pos_;
	LightTRS light_trs_;
	PCamera* camera_;
	/*해당 lightobj를 가지는 오브젝트/객체가 자신의 cb를 얼마나 가지고 있는지 
	(ex. matworld,matview 등을 담당하는 cb1개, g_animation_matrix를 담당하는 cb 1개가 있다면 value = 2;)
	*/
	int numberof_constant_buffer_;
	bool is_rotate_;

public:
	PLightObj();
	~PLightObj();

public:
	D3DXMATRIX mat_normal_;//노말맵에서 사용하는 용도
	bool Init(D3DXVECTOR4 ambient_material, D3DXVECTOR4 ambient_color, D3DXVECTOR4 diffuse_material,
		D3DXVECTOR4 diffuse_color, D3DXVECTOR4 specular_material, D3DXVECTOR4 specular_color, D3DXVECTOR3 position,
		int numberof_cbuffer, ID3D11Device* device,ID3D11DeviceContext* context, PCamera* camera, bool is_rotate);
	bool Frame();
	bool Render();
	bool Release();
	//bool Update();  // updateresources

public:
	D3DXVECTOR3 light_direction();
	D3DXVECTOR3 light_position();
	void set_camera(PCamera* camera);

};

