#pragma once
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <memory>
using std::shared_ptr;
using std::make_shared;
#include <functional>
using std::bind;
using std::function;
using namespace std::placeholders;
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

string tree2paren(shared_ptr<TreeItem> root);

string bintreeArray2paren(BinTreeArray bintree);

void preorder_traversal(shared_ptr<TreeItem> root, function<void(shared_ptr<TreeItem>)> visitFun);

void inorder_traversal(shared_ptr<TreeItem> root, function<void(shared_ptr<TreeItem>)> visitFun);

void postorder_traversal(shared_ptr<TreeItem> root, function<void(shared_ptr<TreeItem>)> visitFun);

void assign_info(shared_ptr<TreeItem> node, bool reset = false);

bool next_bintree(shared_ptr<TreeItem> root);

bool next_bintree_array(BinTreeArray &bintree);
