#include "PDxHelper.h"

namespace DX
{

	ID3D11Buffer* CreateVertexBuffer(ID3D11Device* current_device, const void* vertices, int vertices_count, int vertices_struct_size, bool is_dynamic)
	{

		D3D11_BUFFER_DESC buffer_desc;
		D3D11_SUBRESOURCE_DATA subresource_data;
		ID3D11Buffer* ret_vertex_buffer;

		ZeroMemory(&buffer_desc, sizeof(D3D11_BUFFER_DESC));
		
		if (is_dynamic)
		{
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.CPUAccessFlags = 0;
		}
		
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.ByteWidth = vertices_struct_size * vertices_count;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));
		
		HRESULT hr;
		if (vertices != nullptr)
		{
			subresource_data.pSysMem = vertices;
			hr = current_device->CreateBuffer(&buffer_desc, &subresource_data, &ret_vertex_buffer); //올려볼까 
		}
		else
		{
			hr = current_device->CreateBuffer(&buffer_desc, NULL, &ret_vertex_buffer); //올려볼까 
		}
		

		if (FAILED(hr))
			assert(false);


		return ret_vertex_buffer;
	}

	ID3D11Buffer* CreateIndexBuffer(ID3D11Device* current_device, const void* indices, int indices_count, int indices_struct_size, bool is_dynamic)
	{
		ID3D11Buffer* ret_index_buffer;

		D3D11_BUFFER_DESC index_buf_desc;
		ZeroMemory(&index_buf_desc, sizeof(D3D11_BUFFER_DESC));

		D3D11_SUBRESOURCE_DATA index_buf_init_data;
		ZeroMemory(&index_buf_init_data, sizeof(D3D11_SUBRESOURCE_DATA));
	

		if (is_dynamic)
		{
			index_buf_desc.Usage = D3D11_USAGE_DYNAMIC;
			index_buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			index_buf_desc.Usage = D3D11_USAGE_DEFAULT;
			index_buf_desc.CPUAccessFlags = 0;
		}

		index_buf_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_buf_desc.ByteWidth = indices_struct_size * indices_count;
		index_buf_desc.MiscFlags = 0;


		HRESULT hr;

		if (indices != nullptr)
		{
			index_buf_init_data.pSysMem = indices;
			hr = current_device->CreateBuffer(&index_buf_desc, &index_buf_init_data, &ret_index_buffer);
		}
		else
		{
			hr = current_device->CreateBuffer(&index_buf_desc, NULL, &ret_index_buffer);
		}

		if (FAILED(hr))
			assert(false);

		return ret_index_buffer;

	}

	ID3D11Buffer* CreateConstantBuffer(ID3D11Device* current_device, const void* constant_data, int data_count,int constants_struct_size, bool is_dynamic)
	{
		ID3D11Buffer* ret_constant_buffer;
		D3D11_BUFFER_DESC constant_buf_desc;
		ZeroMemory(&constant_buf_desc, sizeof(D3D11_BUFFER_DESC));


		constant_buf_desc.ByteWidth = constants_struct_size * data_count;
		constant_buf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constant_buf_desc.MiscFlags = 0;
		
		
		if (is_dynamic)
		{
			constant_buf_desc.Usage = D3D11_USAGE_DYNAMIC;
			constant_buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			constant_buf_desc.Usage = D3D11_USAGE_DEFAULT;
			constant_buf_desc.CPUAccessFlags = 0;
		}

		D3D11_SUBRESOURCE_DATA subresource_data;
		ZeroMemory(&subresource_data, sizeof(D3D11_SUBRESOURCE_DATA));
		subresource_data.pSysMem = constant_data;

		HRESULT hr = current_device->CreateBuffer(&constant_buf_desc, &subresource_data, &ret_constant_buffer);


		if (FAILED(hr))
			assert(false);

		return ret_constant_buffer;
		
	}

	ID3D11VertexShader* LoadVertexShaderFromFile(ID3D11Device* current_device, LPCTSTR vs_file_path,
													LPCSTR vs_func_name, bool is_already_compiled, OUT_ ID3DBlob** blob)
	{
		ID3D11VertexShader* ret_vertex_shader = nullptr;
		HRESULT hr;
		ID3DBlob* v_shader_blob = nullptr;
		ID3DBlob* error_msg = nullptr;
		DWORD blob_size = 0;
		LPCVOID blob_data = 0;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )	
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif


		if (is_already_compiled == false)
		{
			hr = D3DX11CompileFromFile(
				vs_file_path, NULL, NULL,
				vs_func_name, "vs_5_0",
				dwShaderFlags, 0, NULL, &v_shader_blob, &error_msg, NULL);


			if (FAILED(hr))
			{
				if (error_msg != nullptr)
				{
					OutputDebugStringA((char*)error_msg->GetBufferPointer());
					if (error_msg)  error_msg->Release();
				}
				assert(false);
				return nullptr;
			}
			blob_size = v_shader_blob->GetBufferSize();
			blob_data = v_shader_blob->GetBufferPointer();
		}
		else //컴파일된 binary blob으로 들어올 경우 
		{
			v_shader_blob = *blob;
			if (v_shader_blob == nullptr) return nullptr;

			blob_size = v_shader_blob->GetBufferSize();
			blob_data = v_shader_blob->GetBufferPointer();
		}

		hr = current_device->CreateVertexShader(blob_data, blob_size, NULL, &ret_vertex_shader);

		if (FAILED(hr))
		{
			v_shader_blob->Release();
			return nullptr;
		}

		if (blob == nullptr)
		{
			v_shader_blob->Release();
		}
		else
		{
			*blob = v_shader_blob;
		}


		return ret_vertex_shader;
	}

	ID3D11PixelShader* LoadPixelShaderFromFile(ID3D11Device* current_device, LPCTSTR ps_file_path, 
													LPCSTR ps_func_name, bool is_already_compiled, OUT_ ID3DBlob** blob)
	{
		ID3D11PixelShader* ret_pixel_shader = nullptr;
		HRESULT hr;
		ID3DBlob* ps_shader_blob = nullptr;
		ID3DBlob* error_msg = nullptr;
		DWORD blob_size = 0;
		LPCVOID blob_data = 0;

		if (is_already_compiled == false)
		{


			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )	
			dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif


			hr = D3DX11CompileFromFile(
				ps_file_path, NULL, NULL,
				ps_func_name, "ps_5_0",
				dwShaderFlags, 0, NULL, &ps_shader_blob, &error_msg, NULL);


			if (FAILED(hr))
			{
				if (error_msg != nullptr)
				{
					OutputDebugStringA((char*)error_msg->GetBufferPointer());
					if (error_msg)  error_msg->Release();
				}
				assert(false);
				return nullptr;
			}
			blob_size = ps_shader_blob->GetBufferSize();
			blob_data = ps_shader_blob->GetBufferPointer();
		}
		else //컴파일된 binary blob으로 들어올 경우 
		{
			ps_shader_blob = *blob;
			if (ps_shader_blob == nullptr) return nullptr;

			blob_size = ps_shader_blob->GetBufferSize();
			blob_data = ps_shader_blob->GetBufferPointer();
		}

		hr = current_device->CreatePixelShader(blob_data, blob_size, NULL, &ret_pixel_shader);

		if (FAILED(hr))
		{
			ps_shader_blob->Release();
			return nullptr;
		}

		if (blob == nullptr)
		{
			ps_shader_blob->Release();
		}
		else
		{
			*blob = ps_shader_blob;
		}


		return ret_pixel_shader;
	}

	ID3D11GeometryShader* LoadGeometryShaderFromFile(ID3D11Device* current_device, LPCTSTR gs_file_path, LPCSTR gs_func_name, bool is_already_compiled, OUT_ ID3DBlob** blob)
	{
		ID3D11GeometryShader* ret_geometry_shader = nullptr;
		HRESULT hr;
		ID3DBlob* gs_shader_blob = nullptr;
		ID3DBlob* error_msg = nullptr;
		DWORD blob_size = 0;
		LPCVOID blob_data = 0;

		if (is_already_compiled == false)
		{


			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )	
			dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif


			hr = D3DX11CompileFromFile(
				gs_file_path, NULL, NULL,
				gs_func_name, "gs_5_0",
				dwShaderFlags, 0, NULL, &gs_shader_blob, &error_msg, NULL);


			if (FAILED(hr))
			{
				if (error_msg != nullptr)
				{
					OutputDebugStringA((char*)error_msg->GetBufferPointer());
					if (error_msg)  error_msg->Release();
				}
				assert(false);
				return nullptr;
			}

			blob_size = gs_shader_blob->GetBufferSize();
			blob_data = gs_shader_blob->GetBufferPointer();
		}
		else //컴파일된 binary blob으로 들어올 경우 
		{
			gs_shader_blob = *blob;
			if (gs_shader_blob == nullptr) return nullptr;

			blob_size = gs_shader_blob->GetBufferSize();
			blob_data = gs_shader_blob->GetBufferPointer();
		}

		hr = current_device->CreateGeometryShader(blob_data, blob_size, NULL, &ret_geometry_shader);

		if (FAILED(hr))
		{
			gs_shader_blob->Release();
			return nullptr;
		}

		if (blob == nullptr)
		{
			gs_shader_blob->Release();
		}
		else
		{
			*blob = gs_shader_blob;
		}


		return ret_geometry_shader;
	}

	ID3D11InputLayout* CreateInputLayout(ID3D11Device* current_device, DWORD vs_blob_size, LPCVOID vs_blob_data, 
												D3D11_INPUT_ELEMENT_DESC* layout, int element_number)
	{
		HRESULT hr;
		ID3D11InputLayout* input_layout = nullptr;

		hr = current_device->CreateInputLayout(layout, element_number, vs_blob_data, vs_blob_size, &input_layout);

		if (FAILED(hr))
		{
			assert(false);
			return nullptr;
		}
	
		return input_layout;

	}



	void PDxHelper::PreRender(ID3D11DeviceContext* context, int stride_length)
	{
		//DX::ApplyBlendState(context, DX::PDxState::blend_state_alphablend_);

		UINT stride = stride_length;
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetConstantBuffers(0, 1, constant_buffer_.GetAddressOf());

/*
		if (shader_res_view_ == nullptr)
			assert(false);*/

		//context->PSSetShaderResources(1, 1, shader_res_view_.GetAddressOf());
		context->IASetInputLayout(input_layout_.Get());
		context->VSSetShader(vertex_shader_.Get(), NULL, 0);
		context->PSSetShader(pixel_shader_.Get(), NULL, 0);
		context->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());

	}

	void PDxHelper::PostRender(ID3D11DeviceContext* context, int index_count)
	{
		if (index_count == 0) index_count = index_count_;
		
		if (index_count != 0)
			context->DrawIndexed(index_count, 0, 0);
		else
			context->Draw(vertex_count_, 0);
	}

	void PDxHelper::Render(ID3D11DeviceContext* context)
	{
		PreRender(context, vertex_size_);
		PostRender(context, index_count_);
	}

	void PDxHelper::set_shader_res_view(ID3D11ShaderResourceView* view)
	{
		shader_res_view_ = view;
	}

}