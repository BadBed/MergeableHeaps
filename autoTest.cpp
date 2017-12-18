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
using std::cout;

extern int random(int max);
extern int random(int min, int max);

namespace test{
const int COUNT_OF_TYPES = 6;

//CEvent, interface
//#############################################################################################

class CEvent{
private:
	int type;//0 addHeap, 1 insert, 2 getMin, 3 popMin, 4 meld, 5 size
	int i, key;

public:
	CEvent (int min_key, int max_key):
		type (0),
		i (0),
		key (random(min_key, max_key)) {}

	CEvent (size_t n, int min_key, int max_key):
		type (random(COUNT_OF_TYPES - 1)),
		i (random(n - 1)),
		key (random(min_key, max_key)) {}

	bool is_correct(vector<size_t>& sizes) const;

	template<class THeap>
	int do_it(vector<IHeap*>& heaps) const;

	string log() const;
};

//implementation
//----------------------------------------------------------------------------------------

bool CEvent::is_correct(vector<size_t>& sizes) const {
	if (type == 4 && i == (int)(sizes.size() - 1)) return false;
	if ((type == 2 || type == 3) && sizes[i] == 0) return false;

	if (type == 0) sizes.push_back(0);
	if (type == 1) ++sizes[i];
	if (type == 3) --sizes[i];
	if (type == 4) {
		sizes[i] += sizes.back();
		sizes.pop_back();
	}
	return true;
}

string CEvent::log() const {
	string result = to_string(type) + " ";

	if (type == 1) {
		result += to_string(i) + " " + to_string(key);	
	}
	else {
		result += to_string(i);
	}
	
	result += "\n";
	return result;
}

template <class THeap>
int CEvent::do_it(vector<IHeap*>& heaps) const {
	if (type == 0) {
		heaps.push_back(new THeap());
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
		heaps[i]->meld(*(heaps.back()));
		heaps.pop_back();
	}
	if (type == 5) {
		return (int)heaps[i]->size();
	}
	return 0;
}

//######################################################################

vector<CEvent> gen_random_events(size_t size, int min_key, int max_key) {
	vector<CEvent> events;
	vector<size_t> sizes;
	events.push_back(CEvent(min_key, max_key));
	events[0].is_correct(sizes);

	while (events.size() < size) {
		CEvent new_event = CEvent(sizes.size(), min_key, max_key);
		if (new_event.is_correct(sizes)) 
			events.push_back(new_event);
	}
	return events;
}

template <class THeap>
class CTester {
private:
	vector<IHeap*> heaps_;
	vector<CEvent> events_;
	int last_test_ = -1;

public:
	CTester (size_t size, int min_key, int max_key):
		events_(gen_random_events(size, min_key, max_key)),
		last_test_(size - 1) {}

	CTester (vector<CEvent> events):
		events_(events),
		last_test_(events.size() - 1){}

	int do_event(int i) {
		last_test_ = i;
		return events_[i].do_it<THeap>(heaps_);
	}

	string log() {
		string result;
		for (int i = 0; i <= last_test_; ++i)
			result += events_[i].log();
		return result;
	}
};

//Correct test
//#################################################################################

template<class THeap, class TSafetyHeap>
bool correct_test(size_t coutnt_of_iter, int min_key, int max_key) {
	vector<CEvent> events = gen_random_events(coutnt_of_iter, min_key, max_key);
	CTester<THeap> tester(events);
	CTester<TSafetyHeap> true_tester(events);

	for (size_t i = 0; i < coutnt_of_iter; ++i) {
		try {
			if (tester.do_event(i) != true_tester.do_event(i)) {
				std::cout << tester.log();
				return false;
			}
		}
		catch (...) {
			std::cout << tester.log();
			return false;
		}
	}
	return true;
}

template<class THeap>
void time_test(const vector<CEvent>& events) {
	CTester<THeap> tester(events);

	time_t start = clock();
	for (size_t i = 0; i < events.size(); ++i) {
		tester.do_event(i);
	}
	time_t finish = clock();
	std::cout << (double)(finish - start)/CLK_TCK << "\n";
}

template<class THeap, class TSafetyHeap>
void many_correct_tests() {
	for (int i = 0; i < 10000; ++i) {
		if (!correct_test<THeap, TSafetyHeap>(20, -10, 10)) {
			cout << "Error on test " << i << "\n";
		}
	}
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

TEST (time, allHeaps) {
	vector<CEvent> events = gen_random_events(1000000, INT_MIN, INT_MAX);
	cout << "Binomial heaps: ";
	time_test<CBinomialHeap>(events);

	cout << "Leftest heaps: ";
	time_test<CLeftestHeap>(events);

	cout << "Splay heaps: ";
	time_test<CSplayHeap>(events);
}

}//test
