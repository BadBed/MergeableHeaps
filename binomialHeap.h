#pragma once
#include "interface.h"
#include <vector>
#include <algorithm>//min
#include <utility>

//Declaration

class CBinomialNode;
class CBinomialTree;
class CBinomialHeap;

//Interface

class CBinomialNode {
public:
	int count_of_child_;
	int key_;
	std::vector<CBinomialNode*> childs_;

	CBinomialNode();
	explicit CBinomialNode(int key);
	~CBinomialNode();
};

class CBinomialTree {
private:
	int order_;
	CBinomialNode* root_;
public:
	CBinomialTree();
	explicit CBinomialTree(int key);
	CBinomialTree(CBinomialNode* root, int order);

	void free();//delete all nodes
	void clear() { root_ = nullptr; }//make heap empty, doesn't delete nodes

	CBinomialHeap destroy();//Delete root, make heap with other elements
	void meld(CBinomialTree& other);

	bool empty() const { return root_ == nullptr; }
	int get_min() const { return root_->key_; }
	int order() const { return order_; }
};

class CBinomialHeap : public IHeap {
private:
	size_t size_;
	std::vector<CBinomialTree> trees_;

	friend class CBinomialTree;

	void push_tree(CBinomialTree tree);
	void add_tree(CBinomialTree& tree);
	int find_min_tree() const ;
public:
	CBinomialHeap();
	~CBinomialHeap();
	
	void insert(int key) override;
	int get_min() const override;
	void pop_min() override;
	void meld(IHeap& iother);

	size_t size() const override{ return size_; }
};

//Implementation
//############################################################################

CBinomialNode::CBinomialNode() :
	count_of_child_(0) {}

CBinomialNode::CBinomialNode(int key) :
	count_of_child_(0),
	key_(key) {}

CBinomialNode::~CBinomialNode() {
	for (int i = 0; i < count_of_child_; i++) {
		if (childs_[i] != nullptr) {
			delete childs_[i];
		}
	}
}

//-----------------------------------------------------------------------------

CBinomialTree::CBinomialTree() :
	root_(nullptr) {}

CBinomialTree::CBinomialTree(int key) :
	order_(0),
	root_(new CBinomialNode(key)) {}

CBinomialTree::CBinomialTree(CBinomialNode* root, int order) :
	order_(order),
	root_(root) {}

void CBinomialTree::free() {
	if (root_ != nullptr)
		delete root_;
}

void CBinomialTree::meld(CBinomialTree& other) {
	if (order_ != other.order_ || empty()) 
		throw "Binominal heap: bad binominal tree meld";

	if (get_min() > other.get_min()) 
		std::swap(root_, other.root_);

	root_->childs_.push_back(other.root_);
	++order_;

	other.clear();
}

CBinomialHeap CBinomialTree::destroy() {
	CBinomialHeap* result = new CBinomialHeap();
	for (int i = 0; i < order_; i++) {
		result->add_tree(CBinomialTree(root_->childs_[i], i));
		root_->childs_[i] = nullptr;
	}

	delete root_;
	clear();
	return (*result);
}

//----------------------------------------------------------------------------

CBinomialHeap::CBinomialHeap() : 
	size_(0){}

CBinomialHeap::~CBinomialHeap() {
	for (size_t i = 0; i < trees_.size(); ++i)
		trees_[i].free();
}

void CBinomialHeap::add_tree(CBinomialTree& tree) {
	if (tree.empty()) return;

	size_ += (size_t)pow(2, tree.order());
	push_tree(tree);

	tree.clear();
}

void CBinomialHeap::push_tree(CBinomialTree tree) {
	int ord = tree.order();

	if (ord >= (int)trees_.size())
		trees_.resize(ord + 1);

	if (trees_[ord].empty()) {
		trees_[ord] = tree;
	}
	else {
		tree.meld(trees_[ord]);
		push_tree(tree);
	}
}

void CBinomialHeap::insert(int key) {
	CBinomialTree tree(key);
	add_tree(tree);
}

int CBinomialHeap::find_min_tree() const {
	if (empty())
		throw "Binominal Heap: find_min_tree used to empty heap";

	int best;
	int result = -1;
	for (size_t i = 1; i < trees_.size(); ++i) {
		if (!trees_[i].empty() && (result == -1 || trees_[i].get_min() < best)) {
			best = trees_[i].get_min();
			result = i;
		}
	}

	return result;
}

int CBinomialHeap::get_min() const {
	int index = find_min_tree();
	return trees_[index].get_min();
}

void CBinomialHeap::meld(IHeap& iother) {
	CBinomialHeap &other = *dynamic_cast<CBinomialHeap*>(&iother);
	for (size_t i = 0; i < other.trees_.size(); i++) {
		add_tree(other.trees_[i]);
	}

	other.size_ = 0;
	other.trees_.clear();
}

void CBinomialHeap::pop_min() {
	int index = find_min_tree();
	CBinomialHeap corpse = trees_[index].destroy();
	size_ -= pow(2, index);
	meld(corpse);
}
