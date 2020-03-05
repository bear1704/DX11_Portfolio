#pragma once
#include "PBitmap.h"
#include "PCollision.h"
#include "PSpriteManager.h"
#include "PObjectInfoManager.h"
#include "PPhysicsModule.h"
#include "P2DCamera.h"
#include "PFsm.h"
#include "PTextureManager.h"


class PObject
{
public:
	PObject();
	virtual ~PObject();
protected:
	PSprite sprite_;
	std::vector<PSprite*> animation_list_;
	pPoint position_;
	FLOAT_RECT collision_box_;
	FLOAT_RECT collision_box_norm_;
	pPoint spawn_position_;
	float alpha_;
	float scale_;
	multibyte_string object_name_;
	PPhysicsModule physics_;
	float gravity_;
	Type type_;
	bool is_reversal_;
	bool invisible_;
	WORD id_;

	///텍스쳐
	DX::PDxHelper dx_helper;
	PTexture* texture;

public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Render(ID3D11DeviceContext* context);
	virtual bool Release();
	virtual void StatusSet(multibyte_string status_path, multibyte_string object_name);
	virtual void StatusSetForServer(multibyte_string status_path, multibyte_string object_name);

	virtual void Set(multibyte_string data_path, multibyte_string object_name, pPoint position);
	
	//getter
	PBitmap* get_bitmap_();
	PBitmap* get_bitmap_mask_();
	PSprite* get_sprite_();
	
	pPoint& get_position_();
	void set_position_(pPoint XY);
	FLOAT_RECT get_collision_rect_();
	FLOAT_RECT get_collision_rect_norm_();
	//bool Load(std::wstring filename);
	void set_collision_box_(FLOAT_RECT norm_box);
	float get_scale_();
	float get_alpha_();
	bool get_invisible_();
	Type get_type_();
	pPoint get_spawn_position_();
	bool& get_is_reversal_();
	multibyte_string get_object_name();
	PPhysicsModule& get_physics_();
	const WORD get_id();
	
	//setter
	void set_sprite_(PSprite& sprite);
	void set_invisible_(bool invisible);
	void set_gravity_(float gravity);
	void set_type_(Type type);
	void set_animation_list_(std::vector<PSprite*> list);
	void set_alpha_and_scale_(float alpha, float scale); //주의! 같은 애니메이션 리스트를 사용하는 모든 개체가 변환이 적용되는 문제가 있음.
	void set_id(WORD id);
	void set_is_reversal(bool isreversal);
	void set_collision_box_norm(FLOAT_RECT fl);

	void Spawn();
	virtual PSprite* find_sprite_by_type(ANIMATIONTYPE type);
	


};