#pragma once
#include "PModel.h"
#include "PSpriteManager.h"
#include "PParser.h"

enum xyz
{
	x = 0,
	y,
	z,
};

struct P_BaseObj
{
	D3DXVECTOR3 pos;
	D3DXMATRIX mat_world;
};

//AABB, OBB 충돌 / 계측 처리를 위한 비렌더링 박스
struct P_BOX : public P_BaseObj
{
	D3DXVECTOR3 center;
	D3DXVECTOR3 box_pos[8];
	//AABB
	D3DXVECTOR3 aabb_min;
	D3DXVECTOR3 aabb_max;
	//OBB
	D3DXVECTOR3 obb_axis[3];
	float obb_extents[3];
};


class PPlaneObject : public PModel
{
public:
	PPlaneObject();
	~PPlaneObject();

public:
	PSprite sprite_;
	float width_;
	float height_;
	std::wstring name;

public:
	//bool DXInit(ID3D11Device* device, ID3D11DeviceContext* context) override;
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring tex_name = L"", std::wstring sprite_name = L"");
	
	bool Frame() override;
	bool Render() override;
public:
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual void CreatePlane(ID3D11Device* device, ID3D11DeviceContext* context, float width, float height, std::wstring sprite_name) final;

};

//center,aabb,obb등 정보가 있으면 그 형태로 Worldmat 생성, 없으면 단위행렬로 사용.
class PBoxObject : public PModel
{
public:
	PBoxObject();
	~PBoxObject();
	
private:
	PSprite sprite_;
public:
	P_BOX box_blueprint_;
	bool is_obb_;

public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring tex_name = L"", std::wstring sprite_name = L"");
	bool OBBInit(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		float ext_x, float ext_y, float ext_z, D3DXVECTOR3 center, D3DXVECTOR3 dir_x, D3DXVECTOR3 dir_y, D3DXVECTOR3 dir_z,
		std::wstring tex_name = L"", std::wstring sprite_name = L"");

	bool Frame() override;
	bool Render() override;

public:
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;

public:
	void KeyRotate(float x, float y, float z);
	void MoveBox(D3DXVECTOR3& dir_vec, float speed);
	void ScaleBox(D3DXVECTOR3& scl_vec);
	bool UpdatePosition();
};


class PImportObject : public PModel
{
public:
	PImportObject();
	~PImportObject();

public:
	std::vector<PGeoMesh> object_list_;
	std::vector<Material> material_list_;
	float elapsed_time_;
	MaxScene scene_;

public:
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context,
		std::wstring vs_file_path, std::string vs_func_name, std::wstring ps_file_path, std::string ps_func_name,
		std::wstring object_path = L"", std::wstring texcomp_path = L"");
	virtual bool Frame() override;
	virtual HRESULT CreateVertexBuffer() override;
	virtual HRESULT CreateIndexBuffer() override;
	virtual HRESULT CreateVertexData() override;
	virtual HRESULT CreateIndexData() override;
	virtual bool PostRender() override;

public:
	bool GetAnimationTrack(const float elapsed_time, const std::vector<PAnimTrack>& tracklist,
		OUT_ PAnimTrack& start_track, OUT_ PAnimTrack& end_track);
	void Interpolate(PGeoMesh& mesh, D3DXMATRIX& mat_parent, float elapsed_time);

};

class PLineObject : public PModel
{
public:
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* context, std::wstring shader_path);
	virtual HRESULT CreateVertexData() override;
	virtual bool PostRender() override;
	void Draw(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR4 color);
public:
	PLineObject();
	virtual ~PLineObject();
};