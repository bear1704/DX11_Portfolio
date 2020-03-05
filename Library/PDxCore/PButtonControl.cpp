#include "PButtonControl.h"
#include "PCollision.h"


PButtonControl::PButtonControl()
{
	state_normal = NORMAL;
	state_hover = HOVER;
	state_push = PUSH;
	state_disable = DISABLE;
	current_sprite_rect = NORMAL;
	is_collision = false;
}


PButtonControl::~PButtonControl()
{

}


bool PButtonControl::Init()
{
	return false;
}
bool PButtonControl::Frame()
{

	/*std::wstring str = L"\ncollision : " + std::to_wstring(collision_box_.left) + L" :   " + 
		std::to_wstring(collision_box_.top) + L" : " + std::to_wstring(collision_box_.right) + L" : " + 
		std::to_wstring(collision_box_.bottom) + L"\npoint : " + std::to_wstring(g_MousePos.x) + L"," + std::to_wstring(g_MousePos.y);
	
	OutputDebugString(str.c_str());*/
	sprite_.Frame();

		current_sprite_rect = NORMAL;
		is_select = false;
		if (PCollision::GetInstance().RectInPoint(collision_box_, g_MousePos))
		{
			if (g_InputActionMap.leftClick == KEYSTAT::KEY_HOLD)
			{
				current_sprite_rect = state_push;
				is_select = true;
			}
			else
			{
				current_sprite_rect = state_hover;
			}
		}



	sprite_.set_current_played_spriteframe_(current_sprite_rect); //현재 상태를 스프라이트 상태로 설정
	set_collision_box_(collision_box_norm_);
	return false;
}
bool PButtonControl::Render()
{
	//sprite_.Draw(is_reversal_);
	return false;
}
bool PButtonControl::Release()
{
	PObject::Release();
	
	return true;
}

PUIComponent * PButtonControl::Clone()
{
	return new PButtonControl(*this);
}
