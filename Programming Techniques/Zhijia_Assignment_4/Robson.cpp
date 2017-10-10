#include "stdafx.h"
#include "Robson.h"
using namespace Robson;
void PathStack::push_back(TreeItem *child)
{
	child->lPtr = parent;
	parent = child;
}

void PathStack::pop_back(TreeItem * child)
{
	//There are three cases, parent has not right child, parent has only one child at its right and parent has two children. Parent's left point retains to its parent while its right point indicates what child it has.
	TreeItem *tmp = parent->lPtr;
	if (parent->rPtr == nullptr)
	{
		//This means parent has not right child, either child is parent's left child or the child itself is a null pointer. Under either case, we could set parent's left point to child.	
		parent->lPtr = child;	
	}
	else if(parent->rPtr == singleRChild)
	{
		//Parent only has one child at its right
		parent->rPtr = child;
		parent->lPtr = nullptr;
	}
	else
	{
		//Parent has two children. In such case, if budy stack's top points to parent, it means parent's troversal has finished, and child is at its right. Otherwise, it means parent's left torversal is completed and child is at its left.
		if (budyStack.back() == parent)
		{

			parent->lPtr = parent->rPtr;
			parent->rPtr = child;
		}
		else
		{
			//Child is parent's left child, switch child with parent's rigt child. 
			//!cannt change child here
			TreeItem *tmp = parent->rPtr;
			parent->rPtr = child;
			child = tmp; budyStack.push_back(parent);
		}
	}
	parent = tmp;
}