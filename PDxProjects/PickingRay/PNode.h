#pragma once
#include "PShape.h"
#include <array>

class PNode
{
public:
	PNode();
	virtual ~PNode();
public:
	vector<PNode*>		child_list_;
	std::array<int,4>	corner_index_;
	P_BOX				box_blueprint_;
	bool				is_leaf_;
	int					depth_;
	vector<P_BaseObj*>	object_list_;
	vector<int>			index_data_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
};

