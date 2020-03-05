#include "PLightObj.h"

PLightObj::PLightObj()
{
	numberof_constant_buffer_ = 0;
}

PLightObj::~PLightObj()
{
}

bool PLightObj::Init(D3DXVECTOR4 ambient_material, D3DXVECTOR4 ambient_color, D3DXVECTOR4 diffuse_material, 
	D3DXVECTOR4 diffuse_color, D3DXVECTOR4 specular_material, D3DXVECTOR4 specular_color, D3DXVECTOR3 position,
	int numberof_thisobj_cbuffer,ID3D11Device* device, ID3D11DeviceContext* context, PCamera* camera, bool is_rotate)
{
	device_ = device;
	context_ = context;
	ZeroMemory(&cb_nearly_not_change_, sizeof(CB_VS_LightNearlyNotChange));
	ZeroMemory(&cb_change_everyframe_, sizeof(CB_VS_ChangesEveryFrame));
	ZeroMemory(&light_direction_, sizeof(D3DXVECTOR3));
	ZeroMemory(&light_trs_, sizeof(LightTRS));
	D3DXMatrixIdentity(&light_world_);
	D3DXMatrixIdentity(&mat_normal_);
	light_trs_.light_trans_.x = position.x;
	light_trs_.light_trans_.y = position.y;
	light_trs_.light_trans_.z = position.z;
	D3DXMatrixIdentity(&light_init_world_);
	D3DXMatrixIdentity(&light_world_);
	
	cb_nearly_not_change_.ambient_material = ambient_material;
	cb_nearly_not_change_.ambient_color = ambient_color;
	cb_nearly_not_change_.diffuse_material = diffuse_material;
	cb_nearly_not_change_.diffuse_color = diffuse_color;
	cb_nearly_not_change_.specular_material = specular_material;
	cb_nearly_not_change_.specular_color = specular_color;
	cb_nearly_not_change_.specualr_intensity = 30.0f;
	cb_nearly_not_change_.is_dirty = false;
	
	camera_ = camera;
	numberof_constant_buffer_ = numberof_thisobj_cbuffer;

	cbuffer_light_nearly_not_changed_.Attach(DX::CreateConstantBuffer(device_, &cb_nearly_not_change_, 1, sizeof(CB_VS_LightNearlyNotChange), true));
	cbuffer_change_every_frame_.Attach(DX::CreateConstantBuffer(device_, &cb_change_everyframe_, 1, sizeof(CB_VS_ChangesEveryFrame), true));

	is_rotate_ = is_rotate;

	return true;
}

bool PLightObj::Frame()
{
	D3DXMatrixTranslation(&light_init_world_, light_trs_.light_trans_.x, light_trs_.light_trans_.y , light_trs_.light_trans_.z);
	D3DXMATRIX mat_rotation;
	D3DXMatrixIdentity(&mat_rotation);
	
	if(is_rotate_)
		D3DXMatrixRotationY(&mat_rotation, g_fGameTimer*1.0f);

	D3DXMatrixMultiply(&light_world_, &light_init_world_, &mat_rotation);

	light_position_.x = light_world_._41;
	light_position_.y = light_world_._42;
	light_position_.z = light_world_._43;

	D3DXVec3Normalize(&light_direction_, &light_position_);
	
	light_direction_ *= -1.0f;

	return true;
}

bool PLightObj::Render()
{
	D3D11_MAPPED_SUBRESOURCE map_subres;
	if (SUCCEEDED(context_->Map((ID3D11Resource*)cbuffer_change_every_frame_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map_subres)))
	{
		CB_VS_ChangesEveryFrame* data = (CB_VS_ChangesEveryFrame*)map_subres.pData;
		data->mat_normal = mat_normal_;
		data->vec_light = light_direction_;
		data->vec_look = camera_->vec_look_;
		data->camera_pos = camera_->camera_position_;

		context_->Unmap(cbuffer_change_every_frame_.Get(), 0);
	}

	//변할때만 업데이트
	if (cb_nearly_not_change_.is_dirty == true)
	{
		D3D11_MAPPED_SUBRESOURCE map_subres1;
		if (SUCCEEDED(context_->Map((ID3D11Resource*)cbuffer_light_nearly_not_changed_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map_subres1)))
		{
			CB_VS_LightNearlyNotChange* data = (CB_VS_LightNearlyNotChange*)map_subres1.pData;
			data->ambient_color = cb_nearly_not_change_.ambient_color;
			data->ambient_material = cb_nearly_not_change_.ambient_material;
			data->diffuse_color = cb_nearly_not_change_.diffuse_color;
			data->diffuse_material = cb_nearly_not_change_.diffuse_material;
			data->specular_color = cb_nearly_not_change_.specular_color;
			data->specular_material = cb_nearly_not_change_.specular_material;
			data->specular_color.w = 20.0f;
			data->is_dirty = false;

			context_->Unmap(cbuffer_light_nearly_not_changed_.Get(), 0);
		}
	}

	context_->VSSetConstantBuffers(numberof_constant_buffer_,     1, cbuffer_change_every_frame_.GetAddressOf());
	context_->VSSetConstantBuffers(numberof_constant_buffer_ + 1, 1, cbuffer_light_nearly_not_changed_.GetAddressOf());
	context_->PSSetConstantBuffers(numberof_constant_buffer_,     1, cbuffer_change_every_frame_.GetAddressOf());
	context_->PSSetConstantBuffers(numberof_constant_buffer_ + 1, 1, cbuffer_light_nearly_not_changed_.GetAddressOf());
	return true;
}

bool PLightObj::Release()
{

	return true;
}

D3DXVECTOR3 PLightObj::light_direction()
{
	return light_direction_;
}

D3DXVECTOR3 PLightObj::light_position()
{
	return light_position_;
}

void PLightObj::set_camera(PCamera* camera)
{
	camera_ = camera;
}
