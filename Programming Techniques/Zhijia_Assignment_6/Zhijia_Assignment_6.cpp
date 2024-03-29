// Zhijia_Assignment_6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lexicographic_bintrees.h"
#include <assert.h>
#include <functional>
#include <vector>
using std::vector;
#include <set>
using std::set;
using std::bind;
using std::function;
using namespace std::placeholders;
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
void assign_strahler_number(shared_ptr<TreeItem> node)
{
	if (node->lPtr&&node->rPtr)
	{
		if (node->lPtr->item == node->rPtr->item)
		{
			node->item = node->lPtr->item + 1;
		}
		else
		{
			node->item = node->lPtr->item > node->rPtr->item ? node->lPtr->item : node->rPtr->item;
		}
	}
	else
	{
		if (node->lPtr || node->rPtr)
		{
			node->item = node->lPtr ? node->lPtr->item : node->rPtr->item;
		}
		else
		{
			node->item = 1;
		}
	}
}

void assign_prunning_number(shared_ptr<TreeItem> node)
{
	if (!node->lPtr)
	{
		node->item = 1;
	}
	else
	{
		shared_ptr<TreeItem> p = node->lPtr;
		unsigned int maxNum = 0, maxCnt = 0;
		while (p)
		{
			if (p->item > maxNum)
			{
				maxNum = p->item;
				maxCnt = 1;
			}
			else if (p->item == maxNum)
			{
				++maxCnt;
			}
			p = p->rPtr;
		}
		node->item = maxCnt > 1 ? maxNum + 1 : maxNum;
	}
}

void array_bintree2link_bintree(shared_ptr<TreeItem> node, BinTreeArray &treeArray)
{
	node->lPtr = treeArray.L.back() > 0 ? make_shared<TreeItem>(treeArray.L.back()) : nullptr;
	treeArray.L.pop_back();
	node->rPtr = treeArray.R.back() > 0 ? make_shared<TreeItem>(treeArray.R.back()) : nullptr;
	treeArray.R.pop_back();
}

int main()
{
	int strahlerNum[10];
	int prunningNum[10];
	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			strahlerNum[j] = 0;
			prunningNum[j] = 0;
		}
		shared_ptr<TreeItem> root = make_shared<TreeItem>(1);
		shared_ptr<TreeItem> p = root;
		for (int j = 0; j < i; ++j)
		{
			p->rPtr = make_shared<TreeItem>(1);
			p = p->rPtr;
		}
		size_t treeCnt = 1;
		set<unsigned int>strahlerNumSet;
		set<unsigned int>prunningNumSet;
		postorder_traversal(root, assign_strahler_number);
		++strahlerNum[root->item];
		inorder_traversal(root, assign_prunning_number);
		int maxPrunNum = 1;
		++prunningNum[maxPrunNum];
		while (next_bintree(root))
		{
			postorder_traversal(root, assign_strahler_number);
			++strahlerNum[root->item];
			inorder_traversal(root, assign_prunning_number);
			maxPrunNum = root->item;
			p = root->rPtr;
			while (p)
			{
				if (p->item > maxPrunNum) maxPrunNum = p->item;
				p = p->rPtr;
			}
			++prunningNum[maxPrunNum];
			++treeCnt;
		}
		cout << "For N = " << i + 1 << ", tree count = " << treeCnt << endl;
		cout << "Possible strahler numbers:"<<endl;
		for (size_t itr = 1; itr != 10; ++itr)
		{
			if (strahlerNum[itr] > 0)
			{
				cout << itr << ":" << strahlerNum[itr] << " bin trees." << endl;
			}
		}
		cout << "Possible prunning numbers:" << endl;
		for (size_t itr = 1; itr != 10; ++itr)
		{
			if (prunningNum[itr] > 0)
			{
				cout << itr << ":" << prunningNum[itr] << " oriented trees." << endl;
			}
		}
		cout << endl;
	}
	getchar();
    return 0;
}

