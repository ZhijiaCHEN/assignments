#pragma once
#include "bin_tree.h"
using namespace BinTree;
#define singleRChild (TreeItem *)0x01
namespace Robson
{
	class KeyStack
	{
	public:
		KeyStack();
		~KeyStack();
		TreeItem *pathStack;
		TreeItem *back();
		bool *empty();
		void push_back(TreeItem *node);
		void pop_back();
	private:

	};

	KeyStack::KeyStack()
	{
	}

	KeyStack::~KeyStack()
	{
	}

	class  PathStack
	{
	public:
		PathStack(KeyStack &);
		~PathStack();
		TreeItem *pathStack;
		TreeItem *back();
		bool *empty();
		void push_back(TreeItem *);
		void pop_back(TreeItem *);
	private:
		TreeItem *parent = nullptr;
		KeyStack &budyStack;
	};

	PathStack::PathStack(KeyStack &keyStack):budyStack(keyStack)
	{
	}
 
	PathStack ::~PathStack()
	{
	}

	
}