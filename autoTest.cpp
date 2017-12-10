#include "pch.h"
#include "allHeaps.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>

using std::vector;
using std::string;
using std::swap;
using std::to_string;

extern int random(int max);
extern int random(int min, int max);

namespace test{
const int COUNT_OF_TYPES = 6;

//CEvent, interface
//#############################################################################################

class CEvent{
private:
	int type;//0 addHeap, 1 insert, 2 getMin, 3 popMin, 4 meld, 5 size
	int i, i2, key;

public:
	CEvent (int min_key, int max_key) {
		type = 0;
		key = random(min_key, max_key);
	}
	CEvent (size_t n, int min_key, int max_key) {
		type = random(COUNT_OF_TYPES - 1);
		i = random(n - 1);
		i2 = random(n - 1);
		key = random(min_key, max_key);
	}

	bool is_correct(vector<size_t>& sizes) const;

	template <class CHeap>
	int operator()(vector<CHeap*>& heaps) const;

	string log() const;
};

//implementation
//----------------------------------------------------------------------------------------

bool CEvent::is_correct(vector<size_t>& sizes) const {
	if (type == 4 && i == i2) return false;
	if ((type == 2 || type == 3) && sizes[i] == 0) return false;

	if (type == 0) sizes.push_back(0);
	if (type == 1) ++sizes[i];
	if (type == 3) --sizes[i];
	if (type == 4) {
		sizes[i] += sizes[i2];
		swap(sizes[i2], sizes.back());
		sizes.pop_back();
	}
	return true;
}

string CEvent::log() const {
	string result = to_string(type) + " ";

	if (type == 1) {
		result += to_string(i) + " " + to_string(key);	
	}
	if (type == 2) {
		result += to_string(i);
	}
	if (type == 3) {
		result += to_string(i);		
	}
	if (type == 4) {
		result += to_string(i) + " " + to_string(i2);
	}
	if (type == 5) {
		result += to_string(i);
	}
	result += "\n";
	return result;
}

template <class CHeap>
int CEvent::operator()(vector<CHeap*>& heaps) const {
	if (type == 0) {
		heaps.push_back(new CHeap());
	}
	if (type == 1) {
		heaps[i]->insert(key);
	}
	if (type == 2) {
		return heaps[i]->get_min();
	}
	if (type == 3) {
		heaps[i]->pop_min();
	}
	if (type == 4) {
		heaps[i]->meld(*heaps[i2]);
		swap(heaps[i2], heaps.back());
		heaps.pop_back();
	}
	if (type == 5) {
		return (int)heaps[i]->size();
	}
	return 0;
}

//--------------------------------------------------------------------------------

vector<CEvent> gen_vector_of_events(size_t size, int min_key, int max_key) {
	vector<CEvent> result;
	vector<size_t> sizes;
	result.push_back(CEvent(min_key, max_key));
	result[0].is_correct(sizes);

	while (result.size() < size) {
		CEvent new_event = CEvent(sizes.size(), min_key, max_key);
		if (new_event.is_correct(sizes)) 
			result.push_back(new_event);
	}
	return result;
}

//Correct test
//##############################################################################################

template<class CHeap, class CSafetyHeap>
bool correct_test(size_t coutnt_of_iter, int min_key, int max_key) {
	vector<CEvent> events = gen_vector_of_events(coutnt_of_iter, min_key, max_key);
	vector<CHeap*> heaps;
	vector<CSafetyHeap*> safety_heaps;
	string log;

	for (size_t i = 0; i < coutnt_of_iter; ++i) {
		log += events[i].log();
		try {
			if (events[i](heaps) != events[i](safety_heaps)) {
				std::cout << log;
				return false;
			}
		}
		catch (...) {
			std::cout << log;
			return false;
		}
	}
	return true;
}

template<class CHeap, class CSafetyHeap>
void many_correct_tests() {
	for (int i = 0; i < 100; ++i)
		correct_test<CHeap, CSafetyHeap>(10, -10, 10);

	for (int i = 0; i < 1000; ++i)
		correct_test<CHeap, CSafetyHeap>(40, INT_MIN, INT_MAX);

	for (int i = 0; i < 100; ++i)
		correct_test<CHeap, CSafetyHeap>(30, INT_MIN, INT_MAX);
}

TEST (correct_auto, splay) {
	many_correct_tests<CSplayHeap, CNativeHeap>();
}

TEST (correct_auto, leftest) {
	many_correct_tests<CLeftestHeap, CNativeHeap>();
}

TEST (correct_auto, binomial) {
	many_correct_tests<CBinomialHeap, CNativeHeap>();
}

}//test
