#pragma once
#include "interface.h"
#include <algorithm>

//Interface

template <class NODE, class IS_SWAP>
class CBinaryHeap : public IHeap {
private:
	NODE* root_;
	size_t size_;
	const IS_SWAP is_swap_;

	NODE* meld_nodes(NODE* first, NODE* second);
public:
	CBinaryHeap() : root_(nullptr), size_(0) {}
	~CBinaryHeap() { if (root_ != nullptr) delete root_; }
	CBinaryHeap& operator=(const CBinaryHeap& other);

	void insert(int key) override;
	int get_min() const override;
	void pop_min() override;
	void meld(IHeap& iother);

	size_t size() const override { return size_; }
};

//---------------------------------------------------------------------------------------------

struct CNodeSplay {
public:
	int key;
	CNodeSplay* left = nullptr;
	CNodeSplay* right = nullptr;

	explicit CNodeSplay(int _key) : key(_key) {}
	~CNodeSplay();
};

struct CNodeLeftest {
public:
	size_t rang;
	int key;
	CNodeLeftest* left = nullptr;
	CNodeLeftest* right = nullptr;

	CNodeLeftest() : rang(0), key(0), left(nullptr), right(nullptr) {}
	explicit CNodeLeftest(int _key) : rang(1), key(_key), left(nullptr), right(nullptr) {}
	~CNodeLeftest();

	friend size_t get_rang(CNodeLeftest* node);
};

size_t get_rang(CNodeLeftest* node);

class CIsSwapSplay {
public:
	CIsSwapSplay() {}
	bool operator()(CNodeSplay* node) const { return true; }
};

class CIsSwapLeftest {
public:
	CIsSwapLeftest() {}
	bool operator()(CNodeLeftest* node) const {
		node->rang = std::max(get_rang(node->left), get_rang(node->right)) + 1;
		return get_rang(node->left) >= get_rang(node->right);
	}
};

//---------------------------------------------------------------------------------------

typedef CBinaryHeap<CNodeSplay, CIsSwapSplay> CSplayHeap;
typedef CBinaryHeap<CNodeLeftest, CIsSwapLeftest> CLeftestHeap;

//Implementation
//######################################################################################

template <class NODE, class IS_SWAP>
CBinaryHeap<NODE, IS_SWAP>&
CBinaryHeap<NODE, IS_SWAP>::operator=(const CBinaryHeap<NODE, IS_SWAP>& other) {
	root_ = other.root_;
	size_ = other.size_;
	return *this;
}

template <class NODE, class IS_SWAP>
NODE* CBinaryHeap<NODE, IS_SWAP>::meld_nodes(NODE* first, NODE* second) {
	if (first == nullptr) return second;
	if (second == nullptr) return first;

	if (second->key < first->key) std::swap(first, second);
	first->right = meld_nodes(first->right, second);
	if (is_swap_(first)) std::swap(first->left, first->right);

	return first;
}

template <class NODE, class IS_SWAP>
void CBinaryHeap<NODE, IS_SWAP>::meld(IHeap& iother) {
	CBinaryHeap<NODE, IS_SWAP> &other = *dynamic_cast<CBinaryHeap<NODE, IS_SWAP>*>(&iother);
	root_ = meld_nodes(root_, other.root_);
	size_ += other.size_;
	other.root_ = nullptr;
	other.size_ = 0;
}

template <class NODE, class IS_SWAP>
void CBinaryHeap<NODE, IS_SWAP>::insert(int key) {
	root_ = meld_nodes(root_, new NODE(key));
	++size_;
}

template <class NODE, class IS_SWAP>
void CBinaryHeap<NODE, IS_SWAP>::pop_min() {
	if (size_ == 0) throw "Mergeable Heaps Exeption: pop_min used to empty heap";
	--size_;

	NODE* old_root = root_;
	root_ = meld_nodes(root_->left, root_->right);

	old_root->left = nullptr;
	old_root->right = nullptr;
	delete old_root;
}

template <class NODE, class IS_SWAP>
int CBinaryHeap<NODE, IS_SWAP>::get_min() const {
	if (size_ == 0) throw "Mergeable Heaps Exeption: get_min used to empty heap";
	return root_->key;
}

//-------------------------------------------------------------------------------------

CNodeSplay::~CNodeSplay() {
	if (left != nullptr) delete left;
	if (right != nullptr) delete right;
}

CNodeLeftest::~CNodeLeftest() {
	if (left != nullptr) delete left;
	if (right != nullptr) delete right;
}

size_t get_rang(CNodeLeftest* node) {
	if (node == nullptr) return 0;
	else return node->rang;
}
