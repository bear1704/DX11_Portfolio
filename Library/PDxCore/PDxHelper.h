#pragma once
#include <dxgi.h>
#include <assert.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <wrl.h>
#include <D3DX10math.h>
#include <D3Dcompiler.h>
#include "PDxState.h"
#include <vector>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "dxgi.lib")

#if defined(_DEBUG)
	#pragma comment (lib, "d3dx10d.lib")
#else
	#pragma comment (lib, "d3dx10.lib")
#endif	

#define OUT_  


struct Vertex_PNCT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR4 color;
	D3DXVECTOR2 uv;

	bool operator == (const Vertex_PNCT& Vertex)
	{
		if (pos == Vertex.pos && normal == Vertex.normal && color == Vertex.color && uv == Vertex.uv)
			return true;

		return false;
	}

	Vertex_PNCT() {};
	Vertex_PNCT(D3DXVECTOR3 v_pos, D3DXVECTOR3 v_normal, D3DXVECTOR4 v_color, D3DXVECTOR2 v_uv)
	{
		pos = v_pos;  normal = v_normal; color = v_color; uv = v_uv;
	};
	Vertex_PNCT(D3DXVECTOR3 v_pos, D3DXVECTOR3 v_normal, D3DXVECTOR4 v_color)
	{
		pos = v_pos;  normal = v_normal; color = v_color;
	}

};




namespace DX
{


	struct PVertexAndUV
	{
		float posX;
		float posY;
		float posZ;
		float u;
		float v;
	};

	struct PVertex
	{
		float poxX;
		float posY;
		float posZ;
	};

	struct PTex_uv
	{
		float u;
		float v;
	};

	//사각형 텍스쳐 바운더리
	struct PTex_uv4
	{
		float u[4];
		float v[4];
	};


	//vertices : 버텍스 버퍼 / vertices_struct_size : 버텍스 버퍼에 사용되는 구조체 사이즈 / vertices_count : 버텍스 개수 / PVertexAndUV 구조체 사용
	ID3D11Buffer*	CreateVertexBuffer(ID3D11Device* current_device,const void* vertices, int vertices_count, int vertices_struct_size, bool is_dynamic);
	//DWORD 구조체 사용
	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* current_device, const void* indices, int indices_count, int indices_struct_size, bool is_dynamic);
	//VS_CONSTANT_BUFFER 구조체 사용
	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* current_device,const void* constant_data , int data_count,int constants_struct_size, bool is_dynamic);
	ID3D11VertexShader* LoadVertexShaderFromFile(ID3D11Device* current_device, LPCTSTR vs_file_path, LPCSTR vs_func_name,
		 bool is_already_compiled, OUT_ ID3DBlob** blob = nullptr);
	ID3D11PixelShader* LoadPixelShaderFromFile(ID3D11Device* current_device, LPCTSTR ps_file_path, LPCSTR ps_func_name,
		bool is_already_compiled, OUT_ ID3DBlob** blob = nullptr);
	ID3D11GeometryShader* LoadGeometryShaderFromFile(ID3D11Device* current_device, LPCTSTR gs_file_path, LPCSTR gs_func_name, 
		bool is_already_compiled, OUT_ ID3DBlob** blob = nullptr);
	ID3D11InputLayout* CreateInputLayout(ID3D11Device* current_device, DWORD vs_blob_size, LPCVOID vs_blob_data,
												D3D11_INPUT_ELEMENT_DESC* layout, int element_number);

	
	

	class PDxHelper
	{
	public:
		int vertex_count_;
		int index_count_;
		int vertex_size_; //버텍스 버퍼 구조체 사이즈

	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer>				vertex_buffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>				index_buffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>				constant_buffer_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertex_shader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixel_shader_;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>		geometry_shader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>			input_layout_;
		Microsoft::WRL::ComPtr<ID3DBlob>					vertex_blob_;
		Microsoft::WRL::ComPtr<ID3DBlob>					geometry_blob_;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_res_view_;

		//Render
		void PreRender(ID3D11DeviceContext* context, int stride_length);
		void PostRender(ID3D11DeviceContext* context, int index_count);
		void Render(ID3D11DeviceContext* context);

		void set_shader_res_view(ID3D11ShaderResourceView* view);
	};


} 