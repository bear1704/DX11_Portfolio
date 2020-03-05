#include "PNormHeightMap.h"

PNormHeightMap::PNormHeightMap()
{
}

PNormHeightMap::~PNormHeightMap()
{
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
