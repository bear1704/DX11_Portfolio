#include "PNode.h"

PNode::PNode()
{
}

PNode::~PNode()
{
	for (int i = 0; i < child_list_.size(); i++)
		delete child_list_[i];

}
