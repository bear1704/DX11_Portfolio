#pragma once
#include "PDxHelper.h"

class PArcBall
{
public:
	PArcBall();
	~PArcBall();


public:
	float mouse_sensitivity_;
	bool is_drag_;
	D3DXVECTOR2 screen_pos_drag_;
	D3DXVECTOR3 screen_drag_angle_;
	D3DXVECTOR3 euler_angle_;
	D3DXMATRIX mat_rotation_;

public:
	D3DXMATRIX GetRotationMatrix();
	void OnBegin(int screen_x, int screen_y);
	void OnMove(int screen_x, int screen_y);
	void OnEnd(int screen_x, int screen_y);




};

