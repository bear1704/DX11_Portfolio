#pragma once
#include "PDevice.h"
#include "PDxHelper.h"

class PTexture
{
public:
	PTexture();
	~PTexture();
	
private:
	ID3D11ShaderResourceView*	shader_res_view_;
	multibyte_string tex_name_;
	multibyte_string tex_path_;
	float width_, height_;
	DX::PTex_uv4 uv_coord_;

public:
	//setter
	void set_shader_res_view(ID3D11ShaderResourceView* view);
	void Apply(ID3D11DeviceContext* device_context, UINT slot_num);
	void Draw(DX::PVertexAndUV vt_uv, float scale, bool is_reversal);
	void set_uv_coord(std::vector<DX::PTex_uv> vec);
	void set_uv_coord(DX::PTex_uv4 uv_coord);
	void SetImageSize(float width, float height);

	//getter
	ID3D11ShaderResourceView*	shader_res_view();
	const ID3D11ShaderResourceView*	shader_res_view_const();

	ID3D11ShaderResourceView**	shader_res_view_double_ptr();
	DX::PTex_uv4& uv_coord();
	float GetImageWidth();
	float GetImageHeight();

	void set_tex_name(multibyte_string name);
	void set_tex_path(multibyte_string path);
	multibyte_string tex_name();
	multibyte_string tex_path();


	

};

