#pragma once
namespace BinTree
{
	struct TreeItem
	{
		TreeItem() {}
		TreeItem(int info) :item(info) {}
		int item = 0;
		TreeItem *lPtr = nullptr;
		TreeItem *rPtr = nullptr;
	};
}
