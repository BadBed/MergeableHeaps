#pragma once
#include "interface.h"
#include <set>

class CNativeHeap : IHeap {
	std::set<int> a;
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
	void meld(CNativeHeap other) {
		a.insert(other.a.begin(), other.a.end());
	}

	virtual size_t size() const override {
		return a.size();
	}
};