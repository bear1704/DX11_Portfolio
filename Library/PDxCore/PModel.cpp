#include "PModel.h"

PModel::PModel() : mytype(FILE_EXTENSION_TYPE::ERROR_OCCUR)
{
	be_using_sprite_ = false;
}

PModel::~PModel()
{
}

bool PModel::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
	D3DXMatrixIdentity(&matWorld_);
	D3DXMatrixIdentity(&matView_);
	D3DXMatrixIdentity(&matProj_);
	device_ = device;
	immediate_context_ = context;
	return true;
}

bool PModel::Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name, std::wstring tex_name, std::wstring sprite_name)
{
	return false;
}

bool PModel::Frame()
{
	return true;
}

bool PModel::Frame(D3DXMATRIX* matrix)
{

	return true;
}

D3DXMATRIX* PModel::FrameMatrix(int start, int end, float elapsed_time)
{
	return nullptr;
}

bool PModel::PreRender()
{
	dx_helper_.PreRender(immediate_context_, dx_helper_.vertex_size_);
	return true;
}

bool PModel::Render()
{
	PreRender();
	PostRender();

	return true;

}

bool PModel::PostRender()
{
	dx_helper_.PostRender(immediate_context_, dx_helper_.index_count_);
	return true;
}

bool PModel::Release()
{
	return false;
}


bool PModel::CreateEx(ID3D11Device* device, ID3D11DeviceContext* context, ShaderFileName filenames,std::wstring tex_name)
{
	if (device_ == nullptr)
		Init(device, context);
	if (immediate_context_ == nullptr)
		Init(device, context);

	if (FAILED(LoadShaderFromFile(device, filenames)))
		return false;
	if (FAILED(CreateInputLayout()))
		return false;
	if (FAILED(CreateVertexData()))
		return false;
	if (FAILED(CreateIndexData()))
		return false;
	if (FAILED(CreateVertexBuffer()))
		return false;
	if (FAILED(CreateIndexBuffer()))
		return false;
	if (FAILED(CreateConstantBuffer()))
		return false;
	if (!UpdateBuffer())
		return false;

	LoadTextures(tex_name); //성공/실패여부 상관없음
}

HRESULT PModel::CreateVertexData()
{
	return S_OK;
}

HRESULT PModel::CreateIndexData()
{
	return S_OK;
}

bool PModel::Create(ID3D11Device* device, ID3D11DeviceContext* context,  std::wstring vs_file_path,
	std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name, std::wstring tex_name)
{
	if (device_ == nullptr)
		Init(device, context);
	if (immediate_context_ == nullptr)
		Init(device, context);

	if(FAILED(LoadVertexShaderFromFile(device_, vs_file_path.c_str(), vs_func_name.c_str(), false, nullptr)))
		return false;
	if (FAILED(LoadPixelShaderFromFile(device_, ps_file_path.c_str(), ps_func_name.c_str(), false, nullptr)))
		return false;
	if (FAILED(CreateInputLayout()))
		return false;
	if (FAILED(CreateVertexData()))
		return false;
	if (FAILED(CreateIndexData()))
		return false;
	if (FAILED(CreateVertexBuffer()))
		return false;
	if (FAILED(CreateIndexBuffer()))
		return false;
	if (FAILED(CreateConstantBuffer()))
		return false;
	if (!UpdateBuffer())
		return false;
	
	LoadTextures(tex_name); //성공/실패여부 상관없음

}

HRESULT PModel::CreateVertexBuffer()
{
	dx_helper_.vertex_size_ = sizeof(Vertex_PNCT);
	dx_helper_.vertex_count_ = vertex_list_.size();
	dx_helper_.vertex_buffer_.Attach(DX::CreateVertexBuffer(device_, &vertex_list_.at(0), dx_helper_.vertex_count_,
		dx_helper_.vertex_size_, false));

	if (dx_helper_.vertex_buffer_.Get() == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT PModel::CreateIndexBuffer()
{
	if (index_list_.size() <= 0) return S_OK;

	dx_helper_.index_count_ = index_list_.size();
	dx_helper_.index_buffer_.Attach(DX::CreateIndexBuffer(
		device_, &index_list_.at(0), dx_helper_.index_count_, sizeof(DWORD), false));

	if (dx_helper_.index_buffer_.Get() == nullptr)
		return E_FAIL;

	return S_OK;

}

HRESULT PModel::CreateConstantBuffer()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	
	
	VS_CB_WVP constant_buffer;
	ZeroMemory(&constant_buffer, sizeof(VS_CB_WVP));
	constant_buffer.matWorld = matWorld;
	constant_buffer.matView = matView_;
	constant_buffer.matProj = matProj_;
	constant_buffer.color[0] = 1.0f;
	constant_buffer.etc[0] = 0.0f;

	dx_helper_.constant_buffer_.Attach(DX::CreateConstantBuffer(device_, &constant_buffer, 1 ,sizeof(VS_CB_WVP), false));

	if (dx_helper_.constant_buffer_.Get() == nullptr)
		return E_FAIL;


	return S_OK;
}

HRESULT PModel::LoadTextures(std::wstring tex_name)
{
	if (tex_name.empty())
		return S_OK;

	PTextureManager::GetInstance().LoadTextureFromScript(L"data/tex.txt", device_);
	texture_ = PTextureManager::GetInstance().GetTextureFromMap(tex_name);

	if (be_using_sprite_)
	{
		DX::PTex_uv4 tex_coord = texture_->uv_coord();

		PModel::ChangeTexValue(vertex_list_, tex_coord);

		dx_helper_.vertex_size_ = sizeof(Vertex_PNCT);
		dx_helper_.vertex_count_ = vertex_list_.size();
		dx_helper_.vertex_buffer_.Attach(DX::CreateVertexBuffer(device_, &vertex_list_.at(0),
			dx_helper_.vertex_count_, dx_helper_.vertex_size_, false));
	}

	if(texture_ != nullptr)
		dx_helper_.shader_res_view_.Attach((texture_->shader_res_view()));

	return S_OK;
}

HRESULT PModel::LoadShaderFromFile(ID3D11Device* current_device, ShaderFileName filenames)
{
	HRESULT hr;
	//VS 파일이름, 함수이름 입력되었을 시
	if (filenames.vs_file_path != L"" && filenames.vs_func_name != "")
	{
		hr = LoadVertexShaderFromFile(current_device, filenames.vs_file_path.c_str(), filenames.vs_func_name.c_str(), false, nullptr);
		if (FAILED(hr))
			return hr;
	}
	if (filenames.ps_file_path != L"" && filenames.ps_func_name!= "")
	{
		hr = LoadPixelShaderFromFile(current_device, filenames.ps_file_path.c_str(), filenames.ps_func_name.c_str(), false, nullptr);
		if (FAILED(hr))
			return hr;
	}
	if (filenames.gs_file_path != L"" && filenames.gs_func_name != "")
	{
		dx_helper_.geometry_shader_.Attach(DX::LoadGeometryShaderFromFile(current_device, filenames.gs_file_path.c_str(), filenames.gs_func_name.c_str(),
			false, nullptr));
	}

	return S_OK;
}

HRESULT PModel::LoadVertexShaderFromFile(ID3D11Device* current_device, LPCTSTR vs_file_path, LPCSTR vs_func_name, bool is_already_compiled, OUT_ ID3DBlob** blob)
{
	dx_helper_.vertex_shader_.Attach(DX::LoadVertexShaderFromFile(current_device,vs_file_path,
		vs_func_name, false, dx_helper_.vertex_blob_.GetAddressOf()));
	return S_OK;
}

HRESULT PModel::LoadPixelShaderFromFile(ID3D11Device* current_device, LPCTSTR ps_file_path, LPCSTR ps_func_name, bool is_already_compiled, OUT_ ID3DBlob** blob)
{
	dx_helper_.pixel_shader_.Attach(DX::LoadPixelShaderFromFile(current_device,ps_file_path, ps_func_name, false));
	return S_OK;
}

bool PModel::UpdateBuffer()
{
	return true;
}

HRESULT PModel::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	int element_count = sizeof(layout) / sizeof(layout[0]);
	dx_helper_.input_layout_.Attach(DX::CreateInputLayout(device_,
		dx_helper_.vertex_blob_->GetBufferSize(), dx_helper_.vertex_blob_->GetBufferPointer(),
		layout, element_count));

	return S_OK;
}

void PModel::SetWVPMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj)
{
	if (world != nullptr)
		matWorld_ = *world;
	if (view != nullptr)
		matView_ = *view;
	if (proj != nullptr)
		matProj_ = *proj;
	
	if (world == nullptr)
		D3DXMatrixIdentity(&matWorld_);

	D3DXMatrixTranspose(&constant_data_.matWorld, &matWorld_);
	D3DXMatrixTranspose(&constant_data_.matView, &matView_);
	D3DXMatrixTranspose(&constant_data_.matProj, &matProj_);

}

void PModel::SetVertexListPos(int count, D3DXVECTOR3 vec)
{
	if (vertex_list_.size() <= count)
		assert(false);

	vertex_list_[count].pos = vec;

}

std::vector<Vertex_PNCT>* PModel::GetVertexListPointer()
{
	return &vertex_list_;
}

std::vector<Vertex_PNCT>& PModel::vertex_list()
{
	return vertex_list_;
}

DX::PDxHelper& PModel::dx_helper()
{
	return dx_helper_;
}



void PModel::ChangeTexValue(OUT_ std::vector<Vertex_PNCT>& vert, const DX::PTex_uv4& tex_buf) noexcept
{
		int count = vert.size();

		for (int i = 0; i < count; i++)
		{
			vert[i].uv.x = tex_buf.u[i%4]; //주의!! Box 처리히려고 바꾼거긴 한데 오류날수도, Plane형태로 구성되는 거면 안날텐데 다른거에선 무조건 날듯
			vert[i].uv.y = tex_buf.v[i%4];
		}

}
