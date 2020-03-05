#pragma once
#include "PStd.h"
#include <D3DX11.h>
#include <D3DX10math.h>
#include "PArcBall.h"
#include "PFrustum.h"

class PCamera
{
public:
	PFrustum frustum_;
	PArcBall arcball_view_;
	PArcBall arcball_world_;
	D3DXVECTOR3 vec_look_;
	D3DXVECTOR3 vec_up_;
	D3DXVECTOR3 vec_right_;
	D3DXVECTOR3 camera_position_;
	D3DXVECTOR3 vec_view_target_;
	D3DXVECTOR4 vec_value_;
	float radius_;

	float pitch_angle_;
	float yaw_angle_;
	float turn_speed_;


	D3DXMATRIX matWorld_;
	D3DXMATRIX matView_;
	D3DXMATRIX matProj_;

	bool render_frustum_;
public:
	PCamera();
	~PCamera();



public:
	void CreateTargetViewMatrix(D3DXVECTOR3 mypos, D3DXVECTOR3 target, D3DXVECTOR3 up);
	void CreateProjectionMatrix();
	//카메라의 단순방향이동에 쓰임
	void MoveCameraVec(D3DXVECTOR3 move = D3DXVECTOR3(0,0,0)); 
	void UpdateVector();
	void SetWVPMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj);

	void UpWard();
	void DownWard();
	void MoveLeft();
	void MoveRight();
	void MoveFoward();
	void MoveBackward();
	void RotateLeft();
	void RotateRight();
	void RotateUp();
	void RotateDown();

	void MessageProc(MSG msg);


public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name);
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render(ID3D11DeviceContext* context);


};

