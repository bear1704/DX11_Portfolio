#include "PNormHeightMap.h"

PNormHeightMap::PNormHeightMap()
{
}

PNormHeightMap::~PNormHeightMap()
{
}

bool PNormHeightMap::Init(ID3D11Device* device, ID3D11DeviceContext* context, PLightObj* obj)
{
	PModel::Init(device, context);
	light_obj_ = obj;

	return false;
}

bool PNormHeightMap::Frame(D3DXVECTOR3 light_dir, D3DXVECTOR3 camera_position, D3DXVECTOR3 vec_look)
{
	//D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
	//if (SUCCEEDED(immediate_context_->Map((ID3D11Resource*)constant_buffer_changes_everyframe_.Get(), 0, D3D11_MAP_WRITE_DISCARD,
	//	0, &MappedFaceDest)))
	//{
	//	CB_VS_ChangesEveryFrame* constant_data = (CB_VS_ChangesEveryFrame*)MappedFaceDest.pData;

	//	constant_data->mat_normal = mat_normal_;
	//	constant_data->light_pos = light_dir;
	//	constant_data->camera_pos = camera_position;
	//	constant_data->vec_look = vec_look;
	//	immediate_context_->Unmap(constant_buffer_changes_everyframe_.Get(), 0);

	//}
	
	light_obj_->Frame();
	return true;
}

bool PNormHeightMap::PreRender()
{
	
	PHeightMap::PreRender();
	ID3D11Buffer* buffer[2] = { dx_helper_.vertex_buffer_.Get(), tangent_space_vbuffer_.Get() };
	UINT stride[2] = { sizeof(Vertex_PNCT), sizeof(D3DXVECTOR3) };
	UINT offset[2] = { 0,0 };

	immediate_context_->IASetVertexBuffers(0, 2, buffer, stride, offset);
	immediate_context_->PSSetShaderResources(1, 1, normal_texture()->shader_res_view_double_ptr());

	light_obj_->mat_normal_ = mat_normal_;
	light_obj_->Render();

	//immediate_context_->VSSetConstantBuffers(1, 1, constant_buffer_changes_everyframe_.GetAddressOf());
	//immediate_context_->VSSetConstantBuffers(2, 1, constant_buffer_nearly_not_changes_.GetAddressOf());
	//immediate_context_->PSSetConstantBuffers(1, 1, constant_buffer_changes_everyframe_.GetAddressOf());
	//immediate_context_->PSSetConstantBuffers(2, 1, constant_buffer_nearly_not_changes_.GetAddressOf());

	return true;

}

bool PNormHeightMap::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool PNormHeightMap::UpdateBuffer()
{
	D3DXVECTOR3 tangent, binormal, normal;

	int i0, i1, i2, i3, i4, i5;

	tangent_list_.resize(dx_helper_.vertex_count_);

	for (int index = 0; index < dx_helper_.vertex_count_; index += 3)
	{
		i0 = index_list_[index + 0];
		i1 = index_list_[index + 1];
		i2 = index_list_[index + 2];

		helper_normalmap_.CreateTangentSpaceVectors(&vertex_list_[i0].pos, &vertex_list_[i1].pos, &vertex_list_[i2].pos,
			vertex_list_[i0].uv, vertex_list_[i1].uv, vertex_list_[i2].uv,
			&tangent, &binormal, &vertex_list_[i0].normal);
		
		tangent_list_[i0] += tangent;

		i0 = index_list_[index + 1];
		i1 = index_list_[index + 2];
		i2 = index_list_[index + 0];

		helper_normalmap_.CreateTangentSpaceVectors(&vertex_list_[i0].pos, &vertex_list_[i1].pos, &vertex_list_[i2].pos,
			vertex_list_[i0].uv, vertex_list_[i1].uv, vertex_list_[i2].uv,
			&tangent, &binormal, &vertex_list_[i0].normal);

		tangent_list_[i0] += tangent;

		i0 = index_list_[index + 2];
		i1 = index_list_[index + 0];
		i2 = index_list_[index + 1];
		
		helper_normalmap_.CreateTangentSpaceVectors(&vertex_list_[i0].pos, &vertex_list_[i1].pos, &vertex_list_[i2].pos,
			vertex_list_[i0].uv, vertex_list_[i1].uv, vertex_list_[i2].uv,
			&tangent, &binormal, &vertex_list_[i0].normal);

		tangent_list_[i0] += tangent;
	}
	for (int i = 0; i < vertex_rows_ * vertex_cols_; i++)
	{
		D3DXVec3Normalize(&tangent_list_[i], &tangent_list_[i]);
	}

	dx_helper_.vertex_buffer_.Attach(DX::CreateVertexBuffer(device_, &vertex_list_.at(0),
		dx_helper_.vertex_count_, dx_helper_.vertex_size_, false));

	UINT numberof_tangent_vertex = tangent_list_.size();
	tangent_space_vbuffer_.Attach(DX::CreateVertexBuffer(device_,
		&tangent_list_.at(0),
		numberof_tangent_vertex,
		sizeof(D3DXVECTOR3), false));

	return true;
}

HRESULT PNormHeightMap::CreateInputLayout()
{

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{

		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,	   1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = sizeof(layout) / sizeof(layout[0]);
	dx_helper_.input_layout_.Attach(DX::CreateInputLayout(device_, dx_helper_.vertex_blob_.Get()->GetBufferSize(),
		dx_helper_.vertex_blob_.Get()->GetBufferPointer(), layout, num_elements));


	return S_OK;
}

HRESULT PNormHeightMap::CreateVertexBuffer()
{
	dx_helper_.vertex_size_ = sizeof(Vertex_PNCT);
	dx_helper_.vertex_count_ = numberof_vertices_;
	return S_OK;
}

HRESULT PNormHeightMap::CreateResource()
{
	//dx_helper_.
	//primitive type을 여기서?
	return E_NOTIMPL;
}

//HRESULT PNormHeightMap::CreateConstantBuffer()
//{
//
//	HRESULT hr;
//	D3D11_BUFFER_DESC cb_everyframe_desc;
//	cb_everyframe_desc.ByteWidth = sizeof(CB_VS_ChangesEveryFrame);
//	cb_everyframe_desc.Usage = D3D11_USAGE_DYNAMIC;
//	cb_everyframe_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	cb_everyframe_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	cb_everyframe_desc.MiscFlags = 0;
//	hr = device_->CreateBuffer(&cb_everyframe_desc, NULL, constant_buffer_changes_everyframe_.GetAddressOf());
//
//	if (FAILED(hr))
//		assert(false);
//
//	cb_everyframe_desc.ByteWidth = sizeof(CB_VS_NearlyNotChange);
//	cb_nearly_not_changes_.cb_AmbientLightColor = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1);
//	cb_nearly_not_changes_.cb_DiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);
//	cb_nearly_not_changes_.cb_SpecularLightColor = D3DXVECTOR4(1, 1, 1, 30.0f);
//
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &cb_nearly_not_changes_;
//	hr = device_->CreateBuffer(&cb_everyframe_desc, &InitData, constant_buffer_nearly_not_changes_.GetAddressOf());
//	if (FAILED(hr))
//		assert(false);
//
//	D3DXMATRIX matWorld;
//	D3DXMatrixIdentity(&matWorld);
//
//
//	VS_CB_WVP constant_buffer;
//	ZeroMemory(&constant_buffer, sizeof(VS_CB_WVP));
//	constant_buffer.matWorld = matWorld;
//	constant_buffer.matView = matView_;
//	constant_buffer.matProj = matProj_;
//	constant_buffer.color[0] = 1.0f;
//	constant_buffer.etc[0] = 0.0f;
//
//	dx_helper_.constant_buffer_.Attach(DX::CreateConstantBuffer(device_, &constant_buffer, 1, sizeof(VS_CB_WVP), false));
//
//	if (dx_helper_.constant_buffer_.Get() == nullptr)
//		return E_FAIL;
//
//
//	return hr;
//
//}

void PNormHeightMap::SetWVPMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj)
{
	if (world != nullptr)
		matWorld_ = *world;
	if (view != nullptr)
		matView_ = *view;
	if (proj != nullptr)
		matProj_ = *proj;


	D3DXMatrixInverse(&mat_normal_, NULL, &matWorld_);

	D3DXMatrixTranspose(&constant_data_.matWorld, &matWorld_);
	D3DXMatrixTranspose(&constant_data_.matView, &matView_);
	D3DXMatrixTranspose(&constant_data_.matProj, &matProj_);
}

void PNormHeightMap::SetNormalTexture(std::wstring tex_path)
{
	PTextureManager::GetInstance().LoadTextureFromScript(L"data/normalmap.txt", device_);
	normal_texture_ = PTextureManager::GetInstance().GetTextureFromMap(tex_path);

	if (normal_texture_ == nullptr)
		assert(false);
}

const D3DXMATRIX& PNormHeightMap::ref_mat_normal()
{
	return mat_normal_;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>* PNormHeightMap::tangent_space_vbuffer()
{
	return &tangent_space_vbuffer_;
}

PTexture* PNormHeightMap::normal_texture()
{
	return normal_texture_;
}
