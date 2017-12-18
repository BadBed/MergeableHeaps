#pragma once
#include "interface.h"
#include <set>

class CNativeHeap : public IHeap {
	std::multiset<int> a;
public:
	void insert(int key) override {
		a.insert(key);
	}
	int get_min() const override {
		return *a.begin();
	}
	void pop_min() override {
		a.erase(a.begin());
	}
	void meld(IHeap& iother) {
		CNativeHeap &other = *dynamic_cast<CNativeHeap*>(&iother);
		a.insert(other.a.begin(), other.a.end());
	}

	virtual size_t size() const override {
		return a.size();
	}
};