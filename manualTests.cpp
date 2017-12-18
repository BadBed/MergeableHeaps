#include "pch.h"
#include <iostream>
#include "allHeaps.h"

template<class Heap>
void test_empty() {
	Heap heap;
	EXPECT_EQ(heap.size(), 0);
}

template<class Heap>
void test_insert() {
	Heap heap;
	heap.insert(6);
	heap.insert(3);
	EXPECT_EQ(heap.get_min(), 3);
}

template<class Heap>
void test_size() {
	Heap heap;
	heap.insert(4);
	heap.insert(7);
	heap.pop_min();
	EXPECT_EQ(heap.size(), 1);
}

template<class Heap>
void test_pop() {
	Heap heap;
	heap.insert(7);
	heap.insert(2);
	heap.insert(3);
	EXPECT_EQ(heap.get_min(), 2);
	heap.pop_min();
	EXPECT_EQ(heap.get_min(), 3);
}

template<class Heap>
void test_meld() {
	IHeap* heap1 = new Heap;
	IHeap* heap2 = new Heap;
	heap1->insert(8);
	heap1->insert(6);
	heap1->insert(-1);
	heap1->insert(5);
	heap2->insert(10);
	heap2->insert(0);
	heap1->meld(*heap2);

	EXPECT_EQ(heap1->get_min(), -1);
	heap1->pop_min();
	EXPECT_EQ(heap1->get_min(), 0);
	delete heap1;
	delete heap2;
}

template<class Heap>
void test_all_manual() {
	test_empty<Heap>();
	test_insert<Heap>();
	test_size<Heap>();
	test_pop<Heap>();
	test_meld<Heap>();
}

TEST(manual, leftest) {
	test_all_manual<CSplayHeap>();
}

TEST(manual, splay) {
	test_all_manual<CLeftestHeap>();
}

TEST(manual, binominal) {
	test_all_manual<CBinomialHeap>();
}

TEST(manual, native) {
	test_all_manual<CNativeHeap>();
}