// Zhijia_Assignment_5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
using std::vector;
#include <iostream>
using std::istream;
using std::cout;
using std::cin;
using std::endl;
#include <fstream>
using std::ifstream;
#include <string>
using std::string;
using std::stoi;
using std::getline;
#include <sstream>
using std::istringstream;
using std::ostringstream;
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <functional>
using std::bind;
using std::function;
using namespace std::placeholders;
using std::size_t;
struct TreeItem
{
	TreeItem() {}
	TreeItem(int info) :item(info) {}
	int item = 0;
	shared_ptr<TreeItem> lPtr = nullptr;
	shared_ptr<TreeItem> rPtr = nullptr;
};

struct BinTreeArray
{
	unsigned int root;
	vector<unsigned int>L;
	vector<unsigned int >R;
};

string tree2paren(shared_ptr<TreeItem> root)
{
	shared_ptr<TreeItem> p = root, q = nullptr, rtptr = nullptr;
	vector<shared_ptr<TreeItem>>s;
	string parenStr;
	while (p || !s.empty())
	{
		if (p)
		{
			parenStr += "(";
			s.push_back(p);
			if (p->lPtr)
			{
				p = p->lPtr;
			}
			else
			{
				parenStr += ")";
				p = p->rPtr;
			}
		}
		else
		{
			parenStr += ")";
			do
			{
				q = s.back();
				s.pop_back();
				if (!s.empty())
				{
					rtptr = s.back()->rPtr;
				}
				else
				{
					rtptr = nullptr;
				}
			} while (!s.empty() && q == rtptr);
			p = rtptr;
		}
	}
	parenStr.pop_back();
	cout << parenStr << endl;
	return parenStr;
}

string bintreeArray2paren(BinTreeArray bintree)
{
	string parenStr;
	vector<string> rParen{ "" };
	vector<size_t>s;
	vector<bool> rSubtree;
	if (bintree.root < 0) return "";
	else parenStr += "(";
	for (size_t i = 0; i < bintree.L.size(); ++i)
	{
		if (bintree.L[i] > 0)
		{
			parenStr += "(";
			if (bintree.R[i] > 0)
			{
				s.push_back(i);
				rParen.push_back("");
				rSubtree.push_back(false);
			}
			else
			{
				rParen[rParen.size()-1] += ")";
			}
		}
		else
		{
			parenStr += ")";
			if (bintree.R[i] > 0)
			{
				parenStr += "(";
			}
			else
			{
				parenStr += (")" + rParen[rParen.size() - 1]);
				rParen.pop_back();
				if (!rSubtree.empty())
				{
					if (!rSubtree.back())
					{
						rSubtree[rSubtree.size() - 1] = true;
						parenStr += "(";
					}
					else
					{
						while (!rSubtree.empty() && rSubtree.back())
						{
							rSubtree.pop_back();
						}
						if (!rSubtree.empty())
						{
							rSubtree[rSubtree.size() - 1] = true;
							parenStr += "(";
						}
					}
				}
				//else
				//{
				//	parenStr += ")";
				//}
			}
		}
	}
	parenStr.pop_back();
	cout << parenStr << endl;
	return parenStr;
}

void preorder_traversal(shared_ptr<TreeItem> root, function<void(shared_ptr<TreeItem>)> visitFun)
{
	shared_ptr<TreeItem> p = root, q = nullptr, rtptr = nullptr;
	vector<shared_ptr<TreeItem>>s;
	while (p || !s.empty())
	{
		if (p)
		{
			visitFun(p);
			s.push_back(p);
			if (p->lPtr)
			{
				p = p->lPtr;
			}
			else
			{
				p = p->rPtr;
			}
		}
		else
		{
			do
			{
				q = s.back();
				s.pop_back();
				if (!s.empty())
				{
					rtptr = s.back()->rPtr;
				}
				else
				{
					rtptr = nullptr;
				}
			} while (!s.empty() && q == rtptr);
			p = rtptr;
		}
	}
}

void assign_info(shared_ptr<TreeItem> node, bool reset = false)
{
	static int cnt = 0;
	if (reset)
	{
		cnt = 0;
		return;
	}
	node->item = ++cnt;
}

bool next_bintree(shared_ptr<TreeItem> root)
{
	//This function transform a binary tree to next binary tree in lexicographic order, return true if transform succeed, false otherwise
	
	//look for the least significant node
	if (!root->lPtr && !root->rPtr) return false;
	shared_ptr<TreeItem> p = root;
	vector<shared_ptr<TreeItem>> s;
	while (p->lPtr || p->rPtr)
	{
		s.push_back(p);
		p = p->rPtr ? p->rPtr : p->lPtr;
	}
	int cnt = 0;
	//count for nodes that will be carried over.
	while (!s.empty() && p == s.back()->lPtr)
	{
		p = s.back();
		s.pop_back();
		++cnt;
	}
	if (s.empty()) return false;
	p = s.back();
	//look for the carry over node
	if (!p->lPtr)
	{
		p->lPtr = make_shared<TreeItem>();
	}
	else
	{
		p = p->lPtr;
		while (p->rPtr)
		{
			p = p->rPtr;
		}
		p->rPtr = make_shared<TreeItem>();
	}
	p = s.back();
	p->rPtr = nullptr;
	//look for the least significant nodes' append position
	p = root;
	while (p->rPtr)
	{
		p = p->rPtr;
	}
	while (cnt > 0)
	{
		p->rPtr = make_shared<TreeItem>();
		p = p->rPtr;
		--cnt;
	}
	return true;
}

bool next_bintree_array(BinTreeArray &bintree)
{
	if (bintree.root == 0 || (bintree.R[0] == 0 && bintree.L[0] == 0)) return false;

	//find the least significant node p and the first fork for breaking from right chain.
	int p = -1, pb = -1, pb0 = -1,  cnt = 0, i = 0;
	while (cnt >= 0)
	{
		if (bintree.L[i] == 0 && bintree.R[i] == 0)
		{
			--cnt;
		}
		else
		{
			if (cnt == 0)
			{
				if (pb < 0 && bintree.R[i] == 0) pb = i;
				if (pb < 0) pb0 = i;
				p = i;
			}
			if (bintree.L[i] > 0 && bintree.R[i] > 0) ++cnt;
		}
		++i;
	}
	//if (bintree.L[p] == 0)
	//{
	//	bintree.L[p] = bintree.R[p];
	//	bintree.R[p] = 0;
	//	return true;
	//}
	//find the nearest right node p0 (could be p itself), which means to find the continuos right parenthesis
	int p0 = p;
	vector<int>ps;//ps stores all the nodes that are going to be carried over
	ps.push_back(p0);
	cnt = 0;
	while (p0 > 0 && bintree.L[p0] > 0 && bintree.R[p0] == 0)
	{
		--p0;
		if (bintree.L[p0] == 0 && bintree.R[p0] == 0)
		{
			++cnt;
			while (cnt > 0)
			{
				--p0;
				if (bintree.L[p0] == 0 && bintree.R[p0] == 0)
				{
					++cnt;
				}
				if (bintree.L[p0] > 0 && bintree.R[p0] > 0)
				{
					--cnt;
				}
			}
		}
		ps.push_back(p0);
	}
	if (bintree.R[ps.back()] == 0) return false;
	if (p0 == pb0) pb = p0;
	////find parent of p0
	//int parent = p0 - 1;
	//if (bintree.L[parent] == 0 && bintree.R[parent] == 0)
	//{
	//	cnt = 1;
	//	while (cnt > 1)
	//	{
	//		--parent;
	//		if (bintree.L[parent] > 0 && bintree.R[parent] > 0) --cnt;
	//		else if (bintree.L[parent] == 0 && bintree.R[parent] == 0) ++cnt;
	//	}
	//}
	//int parentInfo = bintree.R[parent] > 0 ? bintree.R[parent] : bintree.L[parent];
	//
	//if (bintree.L[ps.back()] == 0)
	//{
	//	bintree.L[ps.back()] = parentInfo + 1;
	//}
	//else
	//{
	//	int pc = ps.back() + 1;
	//	parentInfo = 
	//	while (bintree.R[pc] > 0)
	//	{
	//	}
	//}

	//find carry over node
	if (bintree.L[ps.back()] == 0)
	{
		bintree.L[ps.back()] = bintree.R[ps.back()];
	}
	else
	{
		int pc = ps.back() + 1;
		cnt = 0;
		while (bintree.R[pc] > 0 || cnt > 0)
		{
			if (bintree.R[pc] > 0 && bintree.L[pc] > 0) ++cnt;
			else if (bintree.R[pc] == 0 && bintree.L[pc] == 0) --cnt;
			++pc;
		}
		bintree.R[pc] = bintree.R[ps.back()];
	}
	bintree.R[ps.back()] = 0;
	ps.pop_back();
	if (!ps.empty())
	{
		//remove most significant nodes of the remain nodes to pb
		bintree.R[pb] = bintree.L[ps.back()];
		bintree.L[ps.back()] = 0;
		ps.pop_back();
		size_t Idx = bintree.R.size() - 2;
		for (int i = 0; i < ps.size(); ++i, --Idx)
		{
			bintree.R[Idx] = bintree.L[ps[i]];
			bintree.L[ps[i]] = 0;
		}
	}
	return true;
}

void print_bintree_array(BinTreeArray &bintree)
{
	if (bintree.root == 0) return;
	cout << "root: " << bintree.root << endl;
	cout << "L: ";
	for (size_t i = 0; i < bintree.L.size(); ++i)
	{
		cout << bintree.L[i]<<",";
	}
	cout << endl;
	cout << "R: ";
	for (size_t i = 0; i < bintree.R.size(); ++i)
	{
		cout << bintree.R[i] << ",";
	}
	cout << endl;
}

int main()
{
	BinTreeArray test;
	int n;
	cout << "Enter n:" << endl;
	cin >> n;
	if (n <= 0)
	{
		return 0;
	}
	test.root = 1;
	for (int i = 1; i < n;++i)
	{
		test.R.push_back(i+1);
		test.L.push_back(0);
	}
	int cnt1 = 1;
	//while (next_bintree(root))
	//{
	//	tree2paren(root);
	//	++cnt1;
	//}
	//cout << "number of binary trees: " << cnt1 << endl;
	test.R.push_back(0);
	test.L.push_back(0);
	int cnt = 1;
	bintreeArray2paren(test);
	print_bintree_array(test);
	while (next_bintree_array(test))
	{
		bintreeArray2paren(test);
		print_bintree_array(test);
		++cnt;
	}
	cout << "number of binary trees: " << cnt << endl;

	system("pause");
}

