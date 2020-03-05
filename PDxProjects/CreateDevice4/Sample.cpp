#include "Sample.h"


Sample::Sample()
{
	
}

Sample::~Sample()
{

}


bool Sample::Init()
{

	screen_tex_object_.Init(device_, immediate_device_context_, L"VertexShader.hlsl", "VS", L"PixelShader.hlsl", "PS", L"blue");
	obj_.Init(device_, immediate_device_context_, L"VertexShader.hlsl", "VS", L"PixelShader.hlsl", "PS", L"blue");
	box_.Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", "PS", L"tile");
	skybox_.Init(device_, immediate_device_context_, L"Skybox.hlsl", "VS", L"Skybox.hlsl", "PS");


	ship_.Init(device_, immediate_device_context_, L"DiffuseLight.hlsl", "VS", L"DiffuseLight.hlsl", 
		"PS", L"data/obj/ship.PNG", L"data/obj/");

	D3DXMatrixIdentity(&mat_obj_world_);
	D3DXMatrixIdentity(&mat_box_world_);
	
	free_camera_.Init();

	D3DXVECTOR3 eye(0.0f, 0.0f, -2.0f);
	D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);


	free_camera_.CreateTargetViewMatrix(eye, at, up);
	free_camera_.CreateProjectionMatrix();

	D3DXMatrixRotationY(&mat_box_world_, D3DX_PI / 2.5);
	D3DXMatrixScaling(&mat_box_world_, 4.0f, 4.0f, 10.0f);

	mat_obj_world_._42 = 1.0f;
	mat_box_world_._42 = 70.0f;
	

	main_camera_ = &free_camera_;
	

	dx_rt_.Create(device_, 800, 600);
	dx_minimap_rt_.Create(device_, 800, 600);
	
	
	light_obj_.Init(D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1,1,1,1), D3DXVECTOR3(150.0f, 300.0f, 0.0f) ,device_, immediate_device_context_, main_camera_);

	PLightObj* map_light = new PLightObj();
	map_light->Init(D3DXVECTOR4(1.4f, 1.4f, 1.4f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1),
		D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR4(1, 1, 1, 1), D3DXVECTOR3(200.0f, 300.0f, 0.0f), device_, immediate_device_context_, main_camera_);


	map_.Init(device_, immediate_device_context_, map_light);
	map_.CreateHeightMap(device_, immediate_device_context_, L"data/texture/heightMap513.bmp");

	PMapDesc md;
	md.vertex_cols = map_.vertex_cols();
	md.vertex_rows = map_.vertex_rows();
	md.cell_disatnce = 1;
	md.vs_path = L"NormalMap.hlsl";
	md.vs_func = "VS";
	md.ps_path = L"NormalMap.hlsl";
	md.ps_func = "PS";
	md.texture_name = L"block1";

	map_.SetNormalTexture(L"test");
	if (map_.Load(md) == false)
		assert(false);

	//CreateConstantBuffer();

	return true;
}

bool Sample::Frame()
{

	light_obj_.Frame();

#pragma region KEY
	if (g_InputActionMap.qKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->UpWard();
	}
	if (g_InputActionMap.eKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->DownWard();
	}
	if (g_InputActionMap.aKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveLeft();
	}
	if (g_InputActionMap.dKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveRight();
	}
	if (g_InputActionMap.wKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveFoward();
	}
	if (g_InputActionMap.sKey == KEYSTAT::KEY_HOLD)
	{
		main_camera_->MoveBackward();
	}
	if (g_InputActionMap.middleClick == KEYSTAT::KEY_PUSH)
	{
		ToggleIsWireframeRender();
	}
	if (g_InputActionMap.leftArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateLeft();
	if (g_InputActionMap.rightArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateRight();
	if (g_InputActionMap.upArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateUp();
	if (g_InputActionMap.downArrowKey == KEYSTAT::KEY_HOLD)
		main_camera_->RotateDown();

#pragma endregion



	//D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
	//if (SUCCEEDED(immediate_device_context_->Map((ID3D11Resource*)constant_buffer_changes_everyframe_.Get(), 0, D3D11_MAP_WRITE_DISCARD,
	//	0, &MappedFaceDest)))
	//{
	//	CB_VS_ChangesEveryFrame* constant_data = (CB_VS_ChangesEveryFrame*)MappedFaceDest.pData;

	//	constant_data->mat_normal = map_.ref_mat_normal();
	//	constant_data->light_pos = light_obj_.light_direction();
	//	constant_data->camera_pos = main_camera_->camera_position_;
	//	constant_data->vec_look = main_camera_->vec_look_;
	//	immediate_device_context_->Unmap(constant_buffer_changes_everyframe_.Get(), 0);

	//}

	main_camera_->Frame();
	obj_.Frame();
	box_.Frame();
	map_.Frame(light_obj_.light_direction(), main_camera_->camera_position_, main_camera_->vec_look_);
	ship_.Frame();
	return true;
}

bool Sample::Render()
{
	//light

	light_obj_.Render();

	//sky
	D3DXMATRIX mat_sky_world;
	D3DXMatrixScaling(&mat_sky_world, 10, 10, 10);
	D3DXMATRIX mat_sky_view = main_camera_->matView_;
	mat_sky_view._41 = 0.0f;
	mat_sky_view._42 = 0.0f;
	mat_sky_view._43 = 0.0f;

#pragma region SCREEN
	dx_rt_.Begin(immediate_device_context_);
	{
		//DX::ApplyDepthStencilState(immediate_device_context_, DX::PDxState::depth_stencil_state_enable_);

		if(is_wireframe_render_) //마우스 휠 여부에 따라 와이어프레임, 솔리드를 가르는 코드
			DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_wireframe_);
		else
			DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);
		//sky

		DX::ApplyBlendState(immediate_device_context_, DX::PDxState::blend_state_alphablend_);
		DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_wrap_point);
		

		skybox_.SetWVPMatrix(&mat_sky_world, &mat_sky_view, &main_camera_->matProj_);
		skybox_.Render();

		DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_anisotropic);

		obj_.SetWVPMatrix(&mat_obj_world_, &main_camera_->matView_, &main_camera_->matProj_);
		obj_.Render();
		box_.SetWVPMatrix(&mat_box_world_, &main_camera_->matView_, &main_camera_->matProj_);
		box_.Render();

		ship_.SetWVPMatrix(&ship_.object_list_[0].info.meshinfo.world_mat, &main_camera_->matView_, &main_camera_->matProj_);
		//ship_.SetWVPMatrix(&mat_box_world_, &main_camera_->matView_, &main_camera_->matProj_);
		ship_.Render();

		map_.SetWVPMatrix(&main_camera_->matWorld_, &main_camera_->matView_, &main_camera_->matProj_);


		immediate_device_context_->UpdateSubresource(map_.dx_helper_.constant_buffer_.Get(), 0,
			NULL, &map_.constant_data_, 0, 0);
		map_.PreRender();
		
		ID3D11Buffer* buffer[2] = { map_.dx_helper_.vertex_buffer_.Get(), map_.tangent_space_vbuffer_.Get() };
		UINT stride[2] = { sizeof(Vertex_PNCT), sizeof(D3DXVECTOR3) };
		UINT offset[2] = { 0,0 };
		
		immediate_device_context_->IASetVertexBuffers(0, 2, buffer, stride, offset);
		immediate_device_context_->PSSetShaderResources(1, 1, map_.normal_texture()->shader_res_view_double_ptr());
		//immediate_device_context_->VSSetConstantBuffers(1, 1, constant_buffer_changes_everyframe_.GetAddressOf());
		//immediate_device_context_->VSSetConstantBuffers(2, 1, constant_buffer_nearly_not_changes_.GetAddressOf());
		//immediate_device_context_->PSSetConstantBuffers(1, 1, constant_buffer_changes_everyframe_.GetAddressOf());
		//immediate_device_context_->PSSetConstantBuffers(2, 1, constant_buffer_nearly_not_changes_.GetAddressOf());
		map_.PostRender();
	


		dx_rt_.End(immediate_device_context_);
	}
#pragma endregion






#pragma region MINIMAP

	//D3DXMATRIX mat_top_view;
	//D3DXMatrixLookAtLH(&mat_top_view,
	//	&D3DXVECTOR3(0, 70, 0),
	//	&D3DXVECTOR3(0, 0, 0.1),
	//	&D3DXVECTOR3(0, 1, 0));

	//DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);



	//dx_minimap_rt_.Begin(immediate_device_context_);
	//{		
	//	DX::ApplyDepthStencilState(immediate_device_context_, DX::PDxState::depth_stencil_state_enable_);
	//	DX::ApplySamplerState(immediate_device_context_, DX::PDxState::sampler_state_wrap_point);

	//	//skybox_.SetWVPMatrix(&mat_sky_world, &mat_sky_view, &main_camera_->matProj_);
	//	//skybox_.Render();


	//	/*obj_.SetWVPMatrix(&mat_obj_world_, &mat_top_view, &main_camera_->matProj_);
	//	obj_.Render();*/
	//	/*box_.SetWVPMatrix(&mat_box_world_, &mat_top_view, &main_camera_->matProj_);
	//	box_.Render();*/
	//	//map_.SetWVPMatrix(nullptr, &mat_top_view, &main_camera_->matProj_);
	//	//map_.Render();

	//	ship_.SetWVPMatrix(&ship_.object_list_[0].info.meshinfo.world_mat, &main_camera_->matView_, &main_camera_->matProj_);
	//	//ship_.SetWVPMatrix(&mat_box_world_, &main_camera_->matView_, &main_camera_->matProj_);
	//	ship_.Render();

	//	dx_minimap_rt_.End(immediate_device_context_);
	//}


	//screen_tex_object_.vertex_list()[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	//screen_tex_object_.vertex_list()[1].pos = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//screen_tex_object_.vertex_list()[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//screen_tex_object_.vertex_list()[3].pos = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

	//screen_tex_object_.vertex_list()[0].uv = D3DXVECTOR2(0.0f, 0.0f);
	//screen_tex_object_.vertex_list()[1].uv = D3DXVECTOR2(1.0f, 0.0f);
	//screen_tex_object_.vertex_list()[2].uv = D3DXVECTOR2(1.0f, 1.0f);
	//screen_tex_object_.vertex_list()[3].uv = D3DXVECTOR2(0.0f, 1.0f);
	//



	//immediate_device_context_->UpdateSubresource(screen_tex_object_.dx_helper().vertex_buffer_.Get(),
	//	0, NULL, &screen_tex_object_.vertex_list().at(0), 0, 0);
	//screen_tex_object_.SetWVPMatrix(NULL, NULL, NULL);
	//screen_tex_object_.PreRender();
	//immediate_device_context_->PSSetShaderResources(0, 1, dx_minimap_rt_.shader_res_view_.GetAddressOf());
	//screen_tex_object_.PostRender();

#pragma endregion

	DX::ApplyRasterizerState(immediate_device_context_, DX::PDxState::rs_state_solidframe_);
	screen_tex_object_.vertex_list()[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	screen_tex_object_.vertex_list()[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	screen_tex_object_.vertex_list()[2].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	screen_tex_object_.vertex_list()[3].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);

	screen_tex_object_.vertex_list()[0].uv = D3DXVECTOR2(0.0f, 0.0f);
	screen_tex_object_.vertex_list()[1].uv = D3DXVECTOR2(1.0f, 0.0f);
	screen_tex_object_.vertex_list()[2].uv = D3DXVECTOR2(1.0f, 1.0f);
	screen_tex_object_.vertex_list()[3].uv = D3DXVECTOR2(0.0f, 1.0f);

	immediate_device_context_->UpdateSubresource(screen_tex_object_.dx_helper().vertex_buffer_.Get(),
		0, NULL, &screen_tex_object_.vertex_list().at(0), 0, 0);


	screen_tex_object_.SetWVPMatrix(NULL, NULL, NULL);
	screen_tex_object_.PreRender();
	immediate_device_context_->PSSetShaderResources(0, 1,
		dx_rt_.shader_res_view_.GetAddressOf());
	screen_tex_object_.PostRender();

	return true;
}

bool Sample::Release()
{
	obj_.Release();
	box_.Release();
	map_.Release();
	dx_rt_.Release();
	return true;
}

void Sample::MessageProc(MSG msg)
{
	PCore::MessageProc(msg);
	//PInput::GetInstance().MsgProc(msg);
	if (main_camera_ == nullptr) return;
	main_camera_->MessageProc(msg);
}
//
//HRESULT Sample::CreateConstantBuffer()
//{
//	HRESULT hr;
//
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
//	cb_nearly_not_changes_.cb_SpecularLightColor = D3DXVECTOR4(1, 1, 1, 20.0f);
//
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &cb_nearly_not_changes_;
//	hr = device_->CreateBuffer(&cb_everyframe_desc, &InitData, constant_buffer_nearly_not_changes_.GetAddressOf());
//	if (FAILED(hr))
//		assert(false);
//
//	return hr;
//}
