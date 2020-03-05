#include "PImageControl.h"



PImageControl::PImageControl()
{
}


PImageControl::~PImageControl()
{
	
}

bool PImageControl::Init()
{
	
	return false;
}

bool PImageControl::Frame()
{
	sprite_.Frame();
	set_collision_box_(collision_box_norm_);
	return false;
}

bool PImageControl::Render()
{
	//sprite_.AlphaDrawNotCenter(); ///이미지의 경우, Center에서 그리는 기능을 비활성화해야 함(HP-Bar의 양쪽으로 줄어드는 문제 등..)
	return false;
}

bool PImageControl::Release()
{
	PObject::Release();
	return true;
}

void PImageControl::SetRectListSize(FLOAT_RECT size)
{
	//sprite_.set_rect_list_size(size);
}

void PImageControl::set_max_rect_size(FLOAT_RECT size)
{
	max_rect_size = size;
}

FLOAT_RECT & PImageControl::get_max_rect_size()
{
	return max_rect_size;
}

void PImageControl::Draw()
{
	Spawn();
}
