#include "Sample.h"


Sample::Sample()
{
}

Sample::~Sample()
{
}

bool Sample::Init()
{
	std::wstring v_shader_name = L"VertexShader.hlsl";
	std::wstring ps_shader_name = L"PixelShader.hlsl";
	std::string v_shader_func = "VS";
	std::string ps_shader_func = "PS";

	char* aaa = "222";
	std::string aaa;
	

	//PVertexAndUV vertices[] =
	//{
	//	{-0.5f, 0.5f, 0.5f, 0.0f, 0.0f},
	//	{0.5f, 0.5f, 0.5f, 0.5f, 0.0f},
	//	{0.5f, -0.5f, 0.5f, 0.5f, 0.5f},
	//	{-0.5f, -0.5f, 0.5f, 0.0f, 0.5f},
	//}; 

	PTEXTURE_BUF tex_uv[] =
	{
		{0.0f, 0.0f},
		{0.5f, 0.0f},
		{0.5f, 0.5f},
		{0.0f, 0.5f},
	};



	PVertex vertices[] =
	{
		{-0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{-0.5f, -0.5f, 0.5f},
	};

	
	DWORD indices[] =
	{
		0,1,2,
		0,2,3,
	};
	


	InitDevice(hWnd, g_rectangle_client.right, g_rectangle_client.bottom);
	LoadShaderResourceView(L"../../data/bitmap/hots.png", shader_res_view_);
	std::shared_ptr<PVertexAndUV> sptr_tex_vertices(AssemblyVertAndTex(vertices, tex_uv, sizeof(vertices) / sizeof(vertices[0])));

	CreateRenderTarget(g_rectangle_client.right, g_rectangle_client.bottom);
	CreateVertexBuffer(sptr_tex_vertices.get(), 4);
	CreateIndexBuffer(indices, 6);
	CreateConstantBuffer();
	LoadShaderAndInputLayout(v_shader_name.c_str(), ps_shader_name.c_str(), v_shader_func.c_str(), ps_shader_func.c_str());
	

	HRESULT hr;
	D3D11_RASTERIZER_DESC rasterize_desc;
	ZeroMemory(&rasterize_desc, sizeof(rasterize_desc));
	rasterize_desc.DepthClipEnable = TRUE;
	rasterize_desc.FillMode = D3D11_FILL_SOLID;
	rasterize_desc.CullMode = D3D11_CULL_NONE;

	device_->CreateRasterizerState(&rasterize_desc, &state_solid_frame_);

	return true;
}

bool Sample::Frame()
{
	

	float elapsed_time = timer.accumulationTime;
	float bounded_time = cosf(elapsed_time) * 0.5f + 0.5f;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	immediate_device_context_->Map(constant_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	VS_CONSTANT_BUFFER* constant_buf = (VS_CONSTANT_BUFFER*)MappedResource.pData;
	constant_buf->color = VECTOR4{ cosf(elapsed_time), sinf(elapsed_time), 1.0f- cosf(elapsed_time) , 1.0f};
	
	constant_buf->time = bounded_time;
	constant_buf->theta = elapsed_time;
	immediate_device_context_->Unmap(constant_buffer_, 0);

	return true;
}

bool Sample::Render()
{

	DevicePreRender();
	DeviceRender();
	return true;
}

bool Sample::Release()
{

	DeviceRelease();

	return true;
}
