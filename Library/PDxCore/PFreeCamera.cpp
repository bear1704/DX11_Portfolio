#include "PFreeCamera.h"


bool PFreeCamera::Frame()
{
	vec_value_.x = arcball_view_.euler_angle_.x;
	vec_value_.y = arcball_view_.euler_angle_.y;
	vec_value_.z = arcball_view_.euler_angle_.z;
	vec_value_.w = radius_;


	D3DXMATRIX mat_rotation;
	D3DXQUATERNION quat_rotation;
	D3DXQuaternionRotationYawPitchRoll(&quat_rotation,
		vec_value_.y, vec_value_.x, vec_value_.z);
	D3DXMatrixRotationQuaternion(
		&mat_rotation, &quat_rotation);

	mat_rotation._41 = camera_position_.x;
	mat_rotation._42 = camera_position_.y;
	mat_rotation._43 = camera_position_.z;

	D3DXMatrixInverse(&matView_, NULL, &mat_rotation);

	UpdateVector();
	return true;
}

PFreeCamera::PFreeCamera()
{
}

PFreeCamera::~PFreeCamera()
{
}

