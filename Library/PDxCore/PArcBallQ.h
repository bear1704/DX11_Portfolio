#pragma once
#include "PDxHelper.h"

class PArcBallQ
{

public:
	PArcBallQ();
	virtual ~PArcBallQ();

protected:
	int window_width_;
	int window_height_;

	D3DXQUATERNION qt_accumulate_;
	D3DXQUATERNION qt_current_;
	bool is_drag_;
	D3DXMATRIXA16 mat_rotation_;
	D3DXMATRIXA16 mat_translation_;
	D3DXMATRIXA16 mat_translation_delta_;

	D3DXVECTOR3 vec_drag_start_;
	D3DXVECTOR3 vec_drag_current_;
	D3DXVECTOR3 vec_drag_end_;

	float ball_radius_;
	D3DXVECTOR2 ball_center_;




public:
	void OnBegin(int screen_x, int screen_y);
	void OnMove(int screen_x, int screen_y);
	void OnEnd(int screen_x, int screen_y);
	void Reset();
	void SetWindow(int width, int height, float radius);
	D3DXQUATERNION QuatFromBallPoints(const D3DXVECTOR3& vec_from, const D3DXVECTOR3& vec_to);
	D3DXVECTOR3 ScreenToVector(float screen_x, float screen_y);

};

