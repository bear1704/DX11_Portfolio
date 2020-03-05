#include "PCamera.h"

PCamera::PCamera()
{

	D3DXMatrixIdentity(&matWorld_);
	D3DXMatrixIdentity(&matView_);
	D3DXMatrixIdentity(&matProj_);

	radius_ = 5.0f;
	turn_speed_ = 15.0f;

	camera_position_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vec_view_target_ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vec_up_ = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}

PCamera::~PCamera()
{
	
}

void PCamera::CreateTargetViewMatrix(D3DXVECTOR3 mypos, D3DXVECTOR3 target, D3DXVECTOR3 up)
{
	camera_position_ = mypos;
		
	D3DXMatrixLookAtLH(&matView_, &mypos, &target, &up);

	vec_look_ = D3DXVECTOR3(matView_._13, matView_._23, matView_._33);
	vec_up_ = D3DXVECTOR3(matView_._12, matView_._22, matView_._32);
	vec_right_ = D3DXVECTOR3(matView_._11, matView_._21, matView_._31);

	D3DXVec3Normalize(&vec_look_, &vec_look_);
	D3DXVec3Normalize(&vec_up_, &vec_up_);
	D3DXVec3Normalize(&vec_right_, &vec_right_);

	arcball_view_.euler_angle_.y = atan2f(vec_look_.x, vec_look_.z);
	float length = sqrtf(vec_look_.z * vec_look_.z + vec_look_.x * vec_look_.x);
	arcball_view_.euler_angle_.x = -atan2f(vec_look_.y, length);

}

void PCamera::CreateProjectionMatrix()
{
	D3DXMatrixPerspectiveFovLH(&matProj_, D3DX_PI / 4, (float)g_rectangle_client.right / (float)g_rectangle_client.bottom,
	0.1f, 3000.0f);
}



void PCamera::MoveCameraVec(D3DXVECTOR3 move) 
{
	camera_position_ = camera_position_ + move * g_SecondPerFrame * 50.0f;

	float pitch = DEGREE_TO_RADIAN(pitch_angle_);
	float yaw = DEGREE_TO_RADIAN(yaw_angle_);

	vec_view_target_ = camera_position_ + vec_look_;

	matView_._11 = vec_right_.x; matView_._12 = vec_up_.x; matView_._13 = vec_look_.x;
	matView_._21 = vec_right_.y; matView_._22 = vec_up_.y; matView_._23 = vec_look_.y;
	matView_._31 = vec_right_.z; matView_._32 = vec_up_.z; matView_._33 = vec_look_.z;

}

void PCamera::UpdateVector()
{
	vec_look_ = D3DXVECTOR3(matView_._13, matView_._23, matView_._33);
	vec_up_ = D3DXVECTOR3(matView_._12, matView_._22, matView_._32);
	vec_right_ = D3DXVECTOR3(matView_._11, matView_._21, matView_._31);

	D3DXVec3Normalize(&vec_look_, &vec_look_);
	D3DXVec3Normalize(&vec_up_, &vec_up_);
	D3DXVec3Normalize(&vec_right_, &vec_right_);

	frustum_.CreateFrustum(matView_, matProj_);
}

void PCamera::SetWVPMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj)
{
	frustum_.frustum_box_obj_.SetWVPMatrix(world, view, proj);
}

void PCamera::UpWard()
{
	MoveCameraVec(vec_up_);

}

void PCamera::DownWard()
{
	MoveCameraVec(vec_up_ * -1);
}

void PCamera::MoveLeft()
{
	MoveCameraVec(vec_right_ * -1);
}

void PCamera::MoveRight()
{
	MoveCameraVec(vec_right_);
}

void PCamera::MoveFoward()
{
	MoveCameraVec(vec_look_);
}

void PCamera::MoveBackward()
{
	MoveCameraVec(vec_look_ * -1);
}



void PCamera::RotateUp()
{
	pitch_angle_ += turn_speed_ * g_SecondPerFrame;
	MoveCameraVec();
}

void PCamera::RotateDown()
{
	pitch_angle_ += turn_speed_ * g_SecondPerFrame;
	MoveCameraVec();
}

void PCamera::MessageProc(MSG msg)
{
	if (msg.message == WM_LBUTTONDOWN ||
		msg.message == WM_MBUTTONDOWN ||
		msg.message == WM_RBUTTONDOWN)
	{
		int mouse_x = LOWORD(msg.lParam);
		int mouse_y = HIWORD(msg.lParam);
		arcball_view_.OnBegin(mouse_x, mouse_y);
		//arcball_world_.OnBegin(mouse_x, mouse_y);
	}
	if (msg.message == WM_MOUSEMOVE ||
		msg.message == WM_MBUTTONDOWN ||
		msg.message == WM_RBUTTONDOWN)
	{
		int mouse_x = LOWORD(msg.lParam);
		int mouse_y = HIWORD(msg.lParam);
		arcball_view_.OnMove(mouse_x, mouse_y);
		//arcball_world_.OnMove(mouse_x, mouse_y);
	}
	if (msg.message == WM_LBUTTONUP ||
		msg.message == WM_MBUTTONDOWN ||
		msg.message == WM_RBUTTONDOWN)
	{
		int mouse_y = HIWORD(msg.lParam);
		int mouse_x = LOWORD(msg.lParam);
		arcball_view_.OnEnd(mouse_x, mouse_y);
		//arcball_world_.OnEnd(mouse_x, mouse_y);
	}

}

void PCamera::RotateLeft()
{
	yaw_angle_ -= turn_speed_ * g_SecondPerFrame;
	MoveCameraVec();
}

void PCamera::RotateRight()
{
	yaw_angle_ += turn_speed_ * g_SecondPerFrame;
	MoveCameraVec();
}



bool PCamera::Init(ID3D11Device* device, ID3D11DeviceContext* context,
	std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name)
{
	frustum_.frustum_box_obj_.Init(device, context, vs_file_path, "VS_NoLight", ps_file_path, ps_func_name);
	//frustum_.frustum_box_obj_.Init(device, context, vs_file_path, "VS_NoLight", ps_file_path, ps_func_name);
	frustum_.pixel_shader_.Attach(DX::LoadPixelShaderFromFile(device, ps_file_path.c_str(), "PS_Frustum", false));
	render_frustum_ = true;
	return true;
}

bool PCamera::Init()
{
	render_frustum_ = false;
	return true;
}

bool PCamera::Frame()
{
	CreateTargetViewMatrix(camera_position_, vec_view_target_, vec_up_);
	
	return true;
}

bool PCamera::Render(ID3D11DeviceContext* context)
{
	if(render_frustum_)
		frustum_.Render(context);

	return true;
}

