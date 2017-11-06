// ZhijiaAssignment4.cpp : Defines the entry point for the console application.
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
#include <functional>
using std::bind;
using std::function;
using namespace std::placeholders;
#include "bin_tree.h"
using namespace BinTree;
#define singleLChild (TreeItem *)0x01
#define singleRChild (TreeItem *)0x02
//#include "Robson.h"
//using namespace Robson;
//typedef void(visiCall)(TreeItem *);
void pass(TreeItem *rootPtr, istream &inSrc)
{
	return;
}
void extract_tree(TreeItem *nodePtr, istream &inSrc)
{
	if (nodePtr->lPtr) cout << "1 ";
	else cout << "0 ";
	if (nodePtr->rPtr) cout << "1";
	else cout << "0";
	cout << endl;
}
void create(TreeItem *rootPtr, istream &inSrc)
{
	static int info = 0;
	int lFlag, rFlag;
	rootPtr->item = ++info;
	string aLine;
	try
	{
		getline(inSrc, aLine);
		istringstream lineIn(aLine);
		lineIn >> lFlag;
		lineIn >> rFlag;
		rootPtr->lPtr = lFlag ? new TreeItem(lFlag) : nullptr;
		rootPtr->rPtr = rFlag ? new TreeItem(rFlag) : nullptr;
		cout << "input: " << aLine << endl;
	}
	catch (const std::exception& e)
	{
		rootPtr->rPtr = nullptr;
		rootPtr->lPtr = nullptr;
		cout << "Unable to parse input stream: " << e.what() << endl;
	}
	return;
}
TreeItem *create_tree(istream &inSrc)
{
	TreeItem *rootPtr = nullptr, *p = nullptr, *q = nullptr, *rtptr = nullptr;
	vector<TreeItem *>s;
	string aLine;
	int nodeCnt = 0;
	getline(inSrc, aLine);
	istringstream lineIn(aLine);
	int rootFlag;
	lineIn >> rootFlag;
	if (rootFlag == 1)
	{
		rootPtr = new TreeItem();
	}
	p = rootPtr;
	while (p || !s.empty())
	{
		if (p)
		{
			p->item = ++nodeCnt;
			int lFlag, rFlag;
			getline(inSrc, aLine);
			istringstream lineIn(aLine);
			lineIn >> lFlag;
			lineIn >> rFlag;
			p->lPtr = lFlag ? new TreeItem() : nullptr;
			p->rPtr = rFlag ? new TreeItem() : nullptr;
			cout << "input: " << aLine << endl;
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
	return rootPtr;
}
void print_child(TreeItem *nodePtr, istream &inSrc)
{
	if (nodePtr->lPtr)
	{
		cout << nodePtr->lPtr->item;
	}
	else
	{
		cout << "null";
	}
	cout << "<-" << nodePtr->item << "->";
	if (nodePtr->rPtr)
	{
		cout << nodePtr->rPtr->item << endl;
	}
	else
	{
		cout << "null" << endl;
	}
}
void preorder_traversal(TreeItem *root, function<void(TreeItem *)> visitFun)
{
	TreeItem *p = root, *q = nullptr, *rtptr = nullptr;
	vector<TreeItem *>s;
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
void robson_preorder_traversal(TreeItem *root, function<void(TreeItem *)> visitFun)
{
	TreeItem *p = root, *avail = root, *top = nullptr, *pred = nullptr, *stack = nullptr, *tmp, *printPtr = nullptr;
	while (p || pred != nullptr)
	{
		//decend to subtree
		if (p)
		{
			visitFun(p);
			//print stack
			cout << "Visiting node: " << p->item << endl;
			cout << "top points to: ";
			if (top)
			{
				cout << "(";
				if (top->lPtr) cout << top->lPtr->item;
				else cout << "null";
				cout << "<-" << top->item << "->";
				if (top->rPtr) cout << top->rPtr->item;
				else cout << "null";
				cout << ")";
			}
			else
			{
				cout << "null";
			}
			cout << endl;
			cout << "Stack: ";
			printPtr = stack;
			while (printPtr)
			{
				cout << "(";
				if (printPtr->lPtr) cout << printPtr->lPtr->item;
				else cout << "null";
				cout << "<-" << printPtr->item << "->";
				if (printPtr->rPtr) cout << printPtr->rPtr->item;
				else cout << "null";
				cout << "), ";
				//cout << "(stack entry: " << printPtr->item << "->key node: " << printPtr->rPtr->item << ")" << "->";
				printPtr = printPtr->lPtr;
			}
			cout << endl;
			cout << "Back path: ";
			printPtr = pred;
			TreeItem *stackCopy = stack;
			while (printPtr)
			{
				
				cout << "(";
				if (printPtr->lPtr == nullptr) cout << "null";
				else if (printPtr->lPtr == singleRChild) cout << "singleRChild";
				else cout << printPtr->lPtr->item;
				cout << "<-" << printPtr->item << "->";
				if (printPtr->rPtr == nullptr) cout << "null";
				else if (printPtr->rPtr == singleLChild) cout << "singleLChild";
				else cout << printPtr->rPtr->item;
				cout << "), ";
				//cout << "(stack entry: " << printPtr->item << "->key node: " << printPtr->rPtr->item << ")" << "->"; 
				if (top == printPtr || (stackCopy != nullptr && stackCopy->rPtr == printPtr))
				{				
					if (stackCopy->rPtr == printPtr)
					{
						stackCopy = stackCopy->lPtr;
					}
					printPtr = printPtr->rPtr;
				}
				else if (printPtr->lPtr == singleRChild) printPtr = printPtr->rPtr;
				else if (printPtr->rPtr == singleLChild) printPtr = printPtr->lPtr;
				else printPtr = printPtr->lPtr;
			}
			cout << endl;
			if (p->lPtr)
			{
				tmp = p->lPtr;
				p->lPtr = pred;
				p->rPtr = p->rPtr == nullptr ? singleLChild : p->rPtr;
				pred = p;
				p = tmp;
			}
			else
			{
				if (p->rPtr)
				{
					tmp = p->rPtr;
					p->rPtr = pred;
					p->lPtr = singleRChild;
					pred = p;
					p = tmp;
				}
				else
				{
					avail = p;
					p->lPtr = pred;
					pred = p;
					p = nullptr;
				}
			}
		}
		else
		{
			//jump back to root
			do
			{
				if (!pred) return;
				if (pred->lPtr == singleRChild || pred->rPtr == singleLChild || pred->rPtr == nullptr)
				{
					//parent do not have two children.
					if (pred->rPtr == singleLChild)
					{
						//child is at left
						tmp = pred->lPtr;
						pred->lPtr = p;
						pred->rPtr = nullptr;
						p = pred;
						pred = tmp;
					}
					else if (pred->lPtr == singleRChild)
					{
						//child is at right
						tmp = pred->rPtr;
						pred->rPtr = p;
						pred->lPtr = nullptr;
						p = pred;
						pred = tmp;
					}
					else
					{
						//parent has no child
						p = pred;
						pred = pred->lPtr;
						p->lPtr = nullptr;
					}
				}			
				else
				{
					//Parent has two children.
					if (top == pred)
					{
						//Node pred's troversal finished
						tmp = pred->rPtr;
						pred->rPtr = p;
						//step back
						p = pred;
						pred = tmp;
						//pop out stack's top entry
						top = stack->rPtr;
						tmp = stack->lPtr;
						stack->rPtr = nullptr;
						stack->lPtr = nullptr;
						stack = tmp;
					}
					else
					{
						//Child is parent's left child, parent's left subtree finished troversal. Switch child with parent's rigt child and push parent into stack
						tmp = pred->rPtr;
						pred->rPtr = pred->lPtr;
						pred->lPtr = p;
						p = tmp;
						avail->lPtr = stack;
						avail->rPtr = top;
						top = pred;
						//avail->lPtr = stack;
						//avail->rPtr = pred;
						stack = avail;
						break;
					}
				} 
			} while (true);
		}
	}
}
int main()
{
	int nodeCnt = 0;
	ifstream treefile("tree.txt");
	cout << "Creat a tree from tree.txt..." << endl;
	//string aLine; 
	//getline(treefile, aLine);
	//istringstream lineIn(aLine);
    TreeItem *rootPtr = nullptr;
	//int rootFlag;
	//lineIn >> rootFlag;
	rootPtr = create_tree(treefile);
	if (rootPtr)
	{
		//rootPtr = create_tree(treefile);
		//rootPtr = new TreeItem(rootFlag);
		//rootPtr->item = ++nodeCnt;
		//auto callCreate = bind(&create, _1, std::ref(treefile));
		//preorder_traversal(rootPtr, callCreate);
		cout << "echo the tree..." << endl;
		auto callPrint = bind(&extract_tree, _1, std::ref(treefile));
		preorder_traversal(rootPtr, callPrint);
		cout << "Do a modified robson traversal on the tree..." << endl;
		auto callPass = bind(&pass, _1, std::ref(treefile));
		robson_preorder_traversal(rootPtr, callPass);
		cout << "echo the tree aftre robson traversal..." << endl;
		preorder_traversal(rootPtr, callPrint);
	}
	else
	{
		rootPtr = nullptr;
		cout << "a null tree!" << endl;
	}
	
	getchar();
    return 0;
}

