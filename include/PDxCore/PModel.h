#pragma once
#include "PDxHelper.h"
#include "PTextureManager.h"
#include <string>

using namespace Microsoft::WRL;

struct VECTOR4
{
	float x;
	float y;
	float z;
	float w;

};


struct VS_CONSTANT_BUFFER
{
	VECTOR4 color; //c0
	float time; //c1.x
	float theta; //c1.y
	float z; //c1.z
	float w;  //c1.w
};

struct VS_CB_WVP
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;  //주의! 이거 D3DXMatrix였는데 문제생기지않을까?
	float color[4];
	float etc[4];
};

struct ShaderFileName
{
	std::wstring	vs_file_path;
	std::string		vs_func_name;
	std::wstring	ps_file_path;
	std::string		ps_func_name;
	std::wstring	gs_file_path;
	std::string		gs_func_name;

	ShaderFileName()
	{
		vs_file_path = L"";
		vs_func_name = "";
		ps_file_path = L"";
		ps_func_name = "";
		gs_file_path = L"";
		gs_func_name = "";
	}
};

struct PMeshInterface
{
	std::vector<std::vector<int>> indices_list;
	std::vector<DX::PDxHelper> helper_list;

	D3DXMATRIX mat_inverse_world;  //GetNodeTm Inv
	D3DXMATRIX mat_calculation;
	D3DXMATRIX mat_world_self;
	D3DXMATRIX mat_pos;
	D3DXMATRIX mat_rot;
	D3DXMATRIX mat_scale;

	D3DXVECTOR3 pos_track;
	D3DXVECTOR3 scale_track;
	D3DXQUATERNION rot_track;
};

struct PGeoMesh : public PMeshInterface //오브젝트별 geomesh
{
	std::vector<std::vector<Vertex_PNCT>> vertices_list;
	MaxExportInfo info;
	PGeoMesh* parent_geomesh;
};

struct PSkinMesh : public PMeshInterface
{
	std::vector<std::vector<Vertex_PNCTW8I8>> vertices_list;
	PSkinMesh* parent_skinmesh;
	MaxExportSkinInfo info;
};

//animation action 시작/끝 나누기
struct PAction
{
	int start;
	int end;

	struct PAction(int start_, int end_)
	{
		start = start_;
		end = end_;
	}
};

class PModel
{
public:
	D3DXMATRIX matWorld_;
	D3DXMATRIX matView_;
	D3DXMATRIX matProj_;
	D3DXVECTOR3 vec_rotation_for_object_; //오브젝트 회전 시, 회전값을 보존하기 위한 벡터(쿼터니언 -> 오일러 회전값 변환이 너무 어려움..)
	ID3D11Device* device_;
	ID3D11DeviceContext* immediate_context_;
	DX::PDxHelper dx_helper_;
	std::vector<Vertex_PNCT> vertex_list_;
	std::vector<DWORD> index_list_;
	VS_CB_WVP constant_data_;
	PTexture* texture_; //Sprite를 사용하지 않는 보통 오브젝트에서 사용되는 텍스쳐 
	FILE* file_;
	bool be_using_sprite_;
	multibyte_string object_name_;
	FILE_EXTENSION_TYPE mytype;

public:
	PModel();
	virtual ~PModel();

public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring tex_name = L"", std::wstring sprite_name = L"");
	virtual bool Frame();
	virtual bool Frame(D3DXMATRIX* matrix);
	virtual D3DXMATRIX* FrameMatrix(int start, int end, float elapsed_time);
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();
	virtual bool Release();


public:
	/* 지금은 CreateEx를 사용. 이전 코드와의 호환성을 유지하기 위해 정의함 */
	virtual bool Create(ID3D11Device* device, ID3D11DeviceContext* context,  std::wstring vs_file_path, std::string vs_func_name, 
		std::wstring ps_file_path, std::string ps_func_name, std::wstring tex_name = L"");
	/* Create와 같은 기능을 하나, GS 등 추가적인 기능이 정의되어 있음. */
	virtual bool CreateEx(ID3D11Device* device, ID3D11DeviceContext* context, ShaderFileName filenames, std::wstring tex_name = L"");
	virtual HRESULT CreateVertexData();
	virtual HRESULT CreateIndexData();
	//vertices : 버텍스 버퍼 / vertices_struct_size : 버텍스 버퍼에 사용되는 구조체 사이즈 / vertices_count : 버텍스 개수 / PVertexAndUV 구조체 사용
	virtual HRESULT CreateVertexBuffer();
	virtual HRESULT CreateIndexBuffer();
	virtual HRESULT CreateConstantBuffer();
	virtual HRESULT LoadTextures(std::wstring tex_name = nullptr);
	virtual HRESULT LoadShaderFromFile(ID3D11Device* current_device, ShaderFileName filenames);
	virtual HRESULT LoadVertexShaderFromFile(ID3D11Device* current_device, LPCTSTR vs_file_path, LPCSTR vs_func_name,
		bool is_already_compiled, OUT_ ID3DBlob** blob = nullptr);
	virtual HRESULT LoadPixelShaderFromFile(ID3D11Device* current_device, LPCTSTR ps_file_path, LPCSTR ps_func_name,
		bool is_already_compiled, OUT_ ID3DBlob** blob = nullptr);
	virtual bool UpdateBuffer();
	virtual HRESULT CreateInputLayout();
	static void ChangeTexValue(OUT_ std::vector<Vertex_PNCT>& vert, const DX::PTex_uv4& tex_buf) noexcept; 

public:
	//setter
	virtual void SetWVPMatrix(D3DXMATRIX* world = nullptr, D3DXMATRIX* view = nullptr, D3DXMATRIX* proj = nullptr);
	virtual void SetVertexListPos(int count, D3DXVECTOR3 vec);
	//getter
	std::vector<Vertex_PNCT>* GetVertexListPointer();
	std::vector<Vertex_PNCT>& vertex_list();
	DX::PDxHelper& dx_helper();

};

