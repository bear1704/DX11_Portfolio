#include "PArcBall.h"

PArcBall::PArcBall()
{
	mouse_sensitivity_ = 1.0f;
	is_drag_ = false;
	screen_drag_angle_.x = 0;
	screen_drag_angle_.y = 0;
	euler_angle_.x = 0;
	euler_angle_.y = 0;

}

PArcBall::~PArcBall()
{
}

D3DXMATRIX PArcBall::GetRotationMatrix()
{
	D3DXMatrixRotationYawPitchRoll(&mat_rotation_, euler_angle_.y, euler_angle_.x, 0.0f);

	return mat_rotation_;
}

void PArcBall::OnBegin(int screen_x, int screen_y)
{
	is_drag_ = true;
	screen_pos_drag_.x = screen_x;
	screen_pos_drag_.y = screen_y;
}

void PArcBall::OnMove(int screen_x, int screen_y)
{
	if (is_drag_)
	{
		screen_drag_angle_.x = (screen_x - screen_pos_drag_.x);
		screen_drag_angle_.y = (screen_y - screen_pos_drag_.y);
		screen_pos_drag_.x = screen_x;
		screen_pos_drag_.y = screen_y;
		euler_angle_.x += D3DXToRadian(screen_drag_angle_.y) * mouse_sensitivity_;
		euler_angle_.y += D3DXToRadian(screen_drag_angle_.x) * mouse_sensitivity_;
	}
}

void PArcBall::OnEnd(int screen_x, int screen_y)
{
	is_drag_ = false;
}
