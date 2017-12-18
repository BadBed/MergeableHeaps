#pragma once
#include "interface.h"
#include <algorithm>

//Interface

template <class NODE, class UPDATE_NODE>
class CBinaryHeap : public IHeap {
private:
	NODE* root_;
	size_t size_;
	const UPDATE_NODE update_node_;

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

class CUpdateNodeSplay {
public:
	CUpdateNodeSplay() {}
	void operator()(CNodeSplay* node) const {
		std::swap(node->left,node->right);
	}
};

class CUpdateNodeLeftest {
public:
	CUpdateNodeLeftest() {}
	void operator()(CNodeLeftest* node) const {
		node->rang = std::max(get_rang(node->left), get_rang(node->right)) + 1;
		if (get_rang(node->left) >= get_rang(node->right))
			std::swap(node->left,node->right);
	}
};

//---------------------------------------------------------------------------------------

typedef CBinaryHeap<CNodeSplay, CUpdateNodeSplay> CSplayHeap;
typedef CBinaryHeap<CNodeLeftest, CUpdateNodeLeftest> CLeftestHeap;

//Implementation
//######################################################################################

template <class NODE, class UPDATE_NODE>
CBinaryHeap<NODE, UPDATE_NODE>&
CBinaryHeap<NODE, UPDATE_NODE>::operator=(const CBinaryHeap<NODE, UPDATE_NODE>& other) {
	root_ = other.root_;
	size_ = other.size_;
	return *this;
}

template <class NODE, class UPDATE_NODE>
NODE* CBinaryHeap<NODE, UPDATE_NODE>::meld_nodes(NODE* first, NODE* second) {
	if (first == nullptr) return second;
	if (second == nullptr) return first;

	if (second->key < first->key) std::swap(first, second);
	first->right = meld_nodes(first->right, second);
	update_node_(first);

	return first;
}

template <class NODE, class UPDATE_NODE>
void CBinaryHeap<NODE, UPDATE_NODE>::meld(IHeap& iother) {
	CBinaryHeap<NODE, UPDATE_NODE> &other = *dynamic_cast<CBinaryHeap<NODE, UPDATE_NODE>*>(&iother);
	root_ = meld_nodes(root_, other.root_);
	size_ += other.size_;
	other.root_ = nullptr;
	other.size_ = 0;
}

template <class NODE, class UPDATE_NODE>
void CBinaryHeap<NODE, UPDATE_NODE>::insert(int key) {
	root_ = meld_nodes(root_, new NODE(key));
	++size_;
}

template <class NODE, class UPDATE_NODE>
void CBinaryHeap<NODE, UPDATE_NODE>::pop_min() {
	if (size_ == 0) throw "Mergeable Heaps Exeption: pop_min used to empty heap";
	--size_;

	NODE* old_root = root_;
	root_ = meld_nodes(root_->left, root_->right);

	old_root->left = nullptr;
	old_root->right = nullptr;
	delete old_root;
}

template <class NODE, class UPDATE_NODE>
int CBinaryHeap<NODE, UPDATE_NODE>::get_min() const {
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
