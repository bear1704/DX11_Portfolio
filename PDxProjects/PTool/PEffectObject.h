#pragma once
#include "PShape.h"
#include "PSpriteManager.h"
#include <random>

const int kMaxParticle = 1000;


struct PInstance
{
	D3DXMATRIX mat_world;
	D3DXVECTOR4 color;
};

struct PEffectAnimation
{
	D3DXVECTOR3		rotate_axis;
	float			radius;
	bool			clockwise;
	float			current_angle;
	float			speed;
};

class PParticle : public PSprite
{
public:
	PParticle();
	~PParticle();
public:
	EffectInfo effect_info;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 velocity;
	D3DXVECTOR3 gravity;
	D3DXVECTOR3 external_force;

	void CopySprite(PSprite* sprite);

public:
	bool Frame() override;
};

class PEffectObject : public PPlaneObject
{
public:
	EffectInfo					stored_effect_info_; //처음 파티클에 effinfo를 넣을 때 사용하며, 그 이후로는 변경되어선 안 됨
	std::vector<PEffectObject*> eff_list_; //component
	PParticle*					original_particle_; //원형판 파티클
	std::vector<PParticle>		particle_list_;   //실제 렌더되는 파티클 리스트
	std::vector<PInstance>		instance_list_;
	D3DXVECTOR3					world_t_xyz; //반경 회전이 적용된 월드행렬의 위치값

	//animation
	PEffectAnimation			animation_info_;
	bool						is_animate;

	//particle etc..
	float						launch_time;
	D3DXMATRIX					mat_billboard_;
	D3DXMATRIX					plane_rot_matrix_;  //파티클들의 rotation 계산을 위해 저장, 빌보드용
	D3DXVECTOR4					color_;
	D3D11_BLEND					src_blend_;
	D3D11_BLEND					dest_blend_;
	bool						is_use_billboard_;
	bool						is_use_fountain_;
	bool						is_use_alphablend_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_buffer_;
	//Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs_draw; //실제 그릴 버퍼
	//Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs_stream_out; //출력되는 스트림을 저장하는 버퍼 (서로 프레임마다 스왑됨)
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_donothing;

	std::random_device rd;
	std::mt19937 mt;

private:
	float						spawn_time_counter_; //생성 파티클에서, 생성하는 시간 간격
public:
	PEffectObject();
	~PEffectObject();


public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring tex_name, std::wstring sprite_name) override;
	//HRESULT LoadShaderFromFile(ID3D11Device* device_, ShaderFileName filenames) override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;

	virtual HRESULT CreateInputLayout() override;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateVertexBuffer() override;
	bool PostRender() override;


public:
	void AddRandParticle();
	void CreateEffect(ID3D11Device* device, ID3D11DeviceContext* context, float width, float height, std::wstring sprite_name, EffectInfo particle_effect);
	void set_is_multitexture(bool b);
	void set_fadein(float f);
	void set_fadeout(float f);
	void set_mat_bilboard(D3DXMATRIX bill);
	void SetWVPMatrix(D3DXMATRIX* world, D3DXMATRIX* view, D3DXMATRIX* proj) override;
};

