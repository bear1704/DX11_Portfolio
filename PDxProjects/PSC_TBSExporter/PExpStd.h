#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "pch.h"

#define OUT_
using namespace std;

const enum class OBJECT_TYPE
{
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

typedef struct _D3D_MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
}D3D_MATRIX;

struct PNCT
{
	Point3 p;
	Point3 n;
	Point4 c;
	Point2 t;

};

struct TriComponent
{
	int tri_index;
	PNCT  v[3];
};

struct PTextureMap
{
	int map_id;
	TSTR name;
};

struct PMtl
{
	int mtl_id;
	TSTR name;
	std::vector<PTextureMap> tex_list;
	std::vector<PMtl> submaterial_list;

};

struct PAnimTrack
{
	int tick;
	Point3 p;
	Quat q;
};



struct PMesh
{
	TSTR name;
	TSTR parent_name;
	Box3 bounding_box;
	OBJECT_TYPE type;

	Matrix3 world_tm;
	D3D_MATRIX world_d3d;
	D3D_MATRIX world_d3d_inv;

	vector<TriComponent> tri_list; //dummy code?
	vector<vector<TriComponent>> buffer_list;

	int material_id;
	int numberof_submesh;

	vector<vector<PNCT>> vertex_list;
	vector<vector<int>> index_list;

	bool animation_enable[3]; //translate, rotate, scale 애니메이션 존재여부
	std::vector<PAnimTrack> anim_pos;
	std::vector<PAnimTrack> anim_rot;
	std::vector<PAnimTrack> anim_scale;


	PMesh()
	{
		name = L"none";
		parent_name = L"none";
		material_id = -1;
		numberof_submesh = 1;
		type = OBJECT_TYPE::CLASS_GEOM;
	}

};


struct PScene
{
	int first_frame; // 0
	int last_frame; // 100
	int frame_rate; // 30
	int tick_per_frame; // 160
	int numberof_meshes;
	int numberof_materials;
};
