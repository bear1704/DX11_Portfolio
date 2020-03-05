#pragma once
#include "PShape.h"

class PNode
{
public:
	PNode();
	virtual ~PNode();
public:
	vector<PNode*>	child_list_;
	P_BOX			box_blueprint_;
	bool			is_leaf_;
	int				depth_;
	vector<P_BaseObj*>	object_list_;
};

