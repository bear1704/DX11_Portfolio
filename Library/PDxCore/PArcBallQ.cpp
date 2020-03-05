#include "PArcBallQ.h"

PArcBallQ::PArcBallQ()
{
	Reset();
	vec_drag_start_ = D3DXVECTOR3(0, 0, 0);
	vec_drag_current_ = D3DXVECTOR3(0, 0, 0);
	

	RECT rc;
	GetClientRect(GetForegroundWindow(), &rc);
	SetWindow(rc.right, rc.bottom, 0.9f);
}

PArcBallQ::~PArcBallQ()
{
}

void PArcBallQ::OnBegin(int screen_x, int screen_y)
{
	if (screen_x >= 0 && screen_x < window_width_ &&
		screen_y >= 0 && screen_x < window_height_)
	{
		is_drag_ = true;
		qt_accumulate_ = qt_current_;
		vec_drag_start_ = ScreenToVector((float)screen_x, (float)screen_y);

	}

}

void PArcBallQ::OnMove(int screen_x, int screen_y)
{
}

void PArcBallQ::OnEnd(int screen_x, int screen_y)
{
}

void PArcBallQ::Reset()
{
	D3DXQuaternionIdentity(&qt_current_);
	D3DXQuaternionIdentity(&qt_accumulate_);
	D3DXMatrixIdentity(&mat_rotation_);
	D3DXMatrixIdentity(&mat_translation_);
	D3DXMatrixIdentity(&mat_translation_delta_);
	is_drag_ = false;
	ball_radius_ = 1.0f;
}


void PArcBallQ::SetWindow(int width, int height, float radius)
{
	window_width_ = width;
	window_height_ = height;
	ball_radius_ = radius;
	ball_center_ = D3DXVECTOR2(window_width_ / 2.0f, window_height_ / 2.0f);
}

D3DXQUATERNION PArcBallQ::QuatFromBallPoints(const D3DXVECTOR3& vec_from, const D3DXVECTOR3& vec_to)
{
	D3DXVECTOR3 vec_axis;
	float dot = D3DXVec3Dot(&vec_from, &vec_to);
	D3DXVec3Cross(&vec_axis, &vec_from, &vec_to);

	return D3DXQUATERNION(vec_axis.x, vec_axis.y, vec_axis.z, dot);
}

D3DXVECTOR3 PArcBallQ::ScreenToVector(float screen_x, float screen_y)
{



	return D3DXVECTOR3();
}
