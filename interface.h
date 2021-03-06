#pragma once
class IHeap {
public:
	virtual ~IHeap() {};

	virtual void insert(int key) = 0;
	virtual int get_min() const = 0;
	virtual void pop_min() = 0;
	virtual void meld(IHeap& iother) = 0;

	virtual size_t size() const = 0;
	bool empty() const { return size() == 0; }
};
