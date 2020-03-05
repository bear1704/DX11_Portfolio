#include "PMap.h"



bool PMap::Load(PMapDesc& md)
{
	PMapDesc map_desc_ = md;
	vertex_rows_ = md.vertex_rows;
	vertex_cols_ = md.vertex_cols;
	cell_rows_ = md.vertex_rows - 1;
	cell_cols_ = md.vertex_cols - 1;
	numberof_vertices_ = vertex_rows_ * vertex_cols_;
	numberof_faces_ = cell_rows_ * cell_cols_ * 2;
	cell_distance_ = md.cell_disatnce;


	
	if (md.vs_path == L"")
	{
		if (!Create(device_, immediate_context_, L"VertexShader.hlsl", "VS", L"PixelShader.hlsl", "PS", md.texture_name))
			assert(false);
	}
	else
	{
		if (!Create(device_, immediate_context_, md.vs_path, md.vs_func, md.ps_path, md.ps_func, md.texture_name))
			assert(false);
	}

	return true;

}

int PMap::vertex_rows()
{
	return vertex_rows_;
}

int PMap::vertex_cols()
{
	return vertex_cols_;
}

void PMap::InitFaceNormals()
{
	face_normals_.resize(numberof_faces_);
	D3DXVECTOR3 vec = D3DXVECTOR3(0, 1, 0);
	for (int i = 0; i < numberof_faces_; i++)
	{
		face_normals_[i] = vec;
	}
}

D3DXVECTOR3 PMap::ComputeFaceNormal(int idx0, int idx1, int idx2)
{
	D3DXVECTOR3 normal;
	D3DXVECTOR3 v0, v1, v2;

	v0.x = vertex_list_[idx0].pos.x;
	v0.y = vertex_list_[idx0].pos.y;
	v0.z = vertex_list_[idx0].pos.z;
	v1.x = vertex_list_[idx1].pos.x;
	v1.y = vertex_list_[idx1].pos.y;
	v1.z = vertex_list_[idx1].pos.z;
	v2.x = vertex_list_[idx2].pos.x;
	v2.y = vertex_list_[idx2].pos.y;
	v2.z = vertex_list_[idx2].pos.z;

	D3DXVECTOR3 v_edge0 = v1 - v0;
	D3DXVECTOR3 v_edge1 = v2  - v0;
	D3DXVec3Cross(&normal, &v_edge0, &v_edge1);
	D3DXVec3Normalize(&normal, &normal);

	return normal;
}

void PMap::CalcFaceNormals()
{
	
	face_normals_.resize(numberof_faces_);
	int face_count = 0;
	int idx0, idx1, idx2;
	idx0 = idx1 = idx2 = 0;

	for (int idx = 0; idx < numberof_faces_; idx += 3)
	{
		idx0 = index_list_[idx + 0];
		idx1 = index_list_[idx + 1];
		idx2 = index_list_[idx + 2];

		face_normals_[face_count] = ComputeFaceNormal(idx0, idx1, idx2);
		face_count++;
	}

}

PMap::PMap()
{
}

PMap::~PMap()
{
}

HRESULT PMap::CreateVertexData()
{
	float half_cols = (vertex_cols_ - 1) / 2.0f;
	float half_rows = (vertex_rows_ - 1) / 2.0f;
	float tex_offset_u = 1.0f / (vertex_cols_ - 1);
	float tex_offset_v = 1.0f / (vertex_rows_ - 1);

	vertex_list_.resize(numberof_vertices_);

	for (int row = 0; row < vertex_rows_; row++)
	{
		for (int col = 0; col < vertex_cols_; col++)
		{
			int vert_index = row * vertex_cols_ + col;
			vertex_list_[vert_index].pos.x = (col - half_cols) * cell_distance_;
			vertex_list_[vert_index].pos.z = -((row - half_rows) * cell_distance_);
			vertex_list_[vert_index].pos.y = 0.0f;


			vertex_list_[vert_index].uv.x = col * tex_offset_u;
			vertex_list_[vert_index].uv.y = row * tex_offset_v;



			vertex_list_[vert_index].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vertex_list_[vert_index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	return S_OK;
}

HRESULT PMap::CreateIndexData()
{
	index_list_.resize(numberof_faces_ * 3);

	int index = 0;

	for (int row = 0; row < cell_rows_; row++)
	{
		for (int col = 0; col < cell_cols_; col++)
		{
			int next_row = row + 1;
			int next_col = col + 1;

			index_list_[index + 0] = row * vertex_cols_ + col;
			index_list_[index + 1] = row * vertex_cols_ + next_col;
			index_list_[index + 2] = next_row * vertex_cols_ + col;

			int idx0 = index_list_[index + 0];
			int idx1 = index_list_[index + 1];
			int idx2 = index_list_[index + 2];

			D3DXVECTOR3 normal = ComputeFaceNormal(idx0, idx1, idx2);
			vertex_list_[idx0].normal = normal;
			vertex_list_[idx1].normal = normal;
			vertex_list_[idx2].normal = normal;
			D3DXVec3Normalize(&vertex_list_[idx0].normal, &vertex_list_[idx0].normal);
			D3DXVec3Normalize(&vertex_list_[idx1].normal, &vertex_list_[idx1].normal);
			D3DXVec3Normalize(&vertex_list_[idx2].normal, &vertex_list_[idx2].normal);


			index_list_[index + 3] = index_list_[index + 2];
			index_list_[index + 4] = index_list_[index + 1];
			index_list_[index + 5] = next_row * vertex_cols_ + next_col;

			idx0 = index_list_[index + 3];
			idx1 = index_list_[index + 4];
			idx2 = index_list_[index + 5];
			normal = ComputeFaceNormal(idx0, idx1, idx2);
			D3DXVec3Normalize(&vertex_list_[idx0].normal, &vertex_list_[idx0].normal);
			D3DXVec3Normalize(&vertex_list_[idx1].normal, &vertex_list_[idx1].normal);
			D3DXVec3Normalize(&vertex_list_[idx2].normal, &vertex_list_[idx2].normal);
			index += 6;

		}
	}

	return S_OK;
}

bool PHeightMap::CreateHeightMap(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring map_file_path)
{
	D3DX11_IMAGE_LOAD_INFO heightmap_info;
	ZeroMemory(&heightmap_info, sizeof(heightmap_info));
	heightmap_info.MipLevels = 1;
	heightmap_info.Usage = D3D11_USAGE_STAGING;
	heightmap_info.CpuAccessFlags =
		D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	heightmap_info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	HRESULT hr;
	ID3D11Resource* texture_res;
	D3DX11CreateTextureFromFile(
		device, map_file_path.c_str(), &heightmap_info,
		NULL, &texture_res, NULL
	);

	ID3D11Texture2D* tex_2d = (ID3D11Texture2D*)texture_res;

	D3D11_TEXTURE2D_DESC desc_heightmap;
	tex_2d->GetDesc(&desc_heightmap);

	heightmap_tex_list_.resize(desc_heightmap.Width * desc_heightmap.Height);

	D3D11_MAPPED_SUBRESOURCE mapsub;
	if (SUCCEEDED(context->Map(
		tex_2d, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ,
		0, &mapsub
	)))
	{
		UCHAR* texels = (UCHAR*)mapsub.pData;
		for (UINT row = 0; row < desc_heightmap.Height; row++)
		{
			UINT row_start = row * mapsub.RowPitch;
			for (UINT col = 0; col < desc_heightmap.Width; col++)
			{
				UINT red_point = texels[row_start + col * 4];
				heightmap_tex_list_[row * desc_heightmap.Width + col] = red_point;
			}
		}

		context->Unmap(tex_2d, D3D11CalcSubresource(0, 0, 1));
	}

	vertex_rows_ = desc_heightmap.Height;
	vertex_cols_ = desc_heightmap.Width;


	texture_res->Release();

	return true;
}

HRESULT PHeightMap::CreateVertexData()
{
	float half_cols = (vertex_cols_ - 1) / 2.0f;
	float half_rows = (vertex_rows_ - 1) / 2.0f;
	float tex_offset_u = 1.0f / (vertex_cols_ - 1);
	float tex_offset_v = 1.0f / (vertex_rows_ - 1);

	vertex_list_.resize(numberof_vertices_);

	for (int row = 0; row < vertex_rows_; row++)
	{
		for (int col = 0; col < vertex_cols_; col++)
		{
			int vert_index = row * vertex_cols_ + col;
			vertex_list_[vert_index].pos.x = (col - half_cols) * cell_distance_;
			vertex_list_[vert_index].pos.z = -((row - half_rows) * cell_distance_);
			vertex_list_[vert_index].pos.y = GetHeightMapList(vert_index);


			vertex_list_[vert_index].uv.x = col * tex_offset_u;
			vertex_list_[vert_index].uv.y = row * tex_offset_v;

			//vertex_list_[vert_index].uv.x = col;
			//vertex_list_[vert_index].uv.y = row;

			vertex_list_[vert_index].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			vertex_list_[vert_index].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			
		}
	}

	return S_OK;
}

PHeightMap::PHeightMap()
{
}

PHeightMap::~PHeightMap()
{
}
