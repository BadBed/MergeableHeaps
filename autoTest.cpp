#include "pch.h"
#include "allHeaps.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <map>

using std::vector;
using std::string;
using std::swap;
using std::to_string;
using std::cout;

extern int random(int max);
extern int random(int min, int max);

namespace test{
//CEvent, interface
//#############################################################################################

enum CEventType { ADD_HEAP = 0, INSERT, GET_MIN, POP_MIN, MELD, SIZE, COUNT_OF_TYPES };
#define FIRST_EVENT_TYPE ADD_HEAP

//###########################################################################################

struct CTypesChances {
private:
	std::map<CEventType, double> a;
	void norm() {
		double sum = 0.0;
		for (int i = 0; i < COUNT_OF_TYPES; ++i) {
			CEventType x = static_cast<CEventType>(i);
			sum += a[x];
		}
		if (sum == 0.0) {
			return;
		}
		for (int i = 0; i < COUNT_OF_TYPES; ++i) {
			CEventType x = static_cast<CEventType>(i);
			a[x] /= sum;
		}
	}
public:
	CTypesChances() {
		for (int i = 0; i < COUNT_OF_TYPES; ++i) {
			CEventType x = static_cast<CEventType>(i);
			a[x] = 1.0;
		}
		norm();
	}
	void change(CEventType event_type, double multiplicator) {
		a[event_type] *= multiplicator;
		norm();
	}
	CEventType get_rand() {
		double rnd = (double)random(1e7) / 1e7;
		double sum = 0.0;
		for (int i = 0; i < COUNT_OF_TYPES; ++i) {
			CEventType x = static_cast<CEventType>(i);
			sum += a[x];
			if (sum >= rnd)
				return x;
		}
		assert(false);
	}
};

//########################################################################################

class CEvent{
public:
	CEventType type;//0 addHeap, 1 insert, 2 getMin, 3 popMin, 4 meld, 5 size
	int i, key;

	CEvent (int min_key, int max_key):
		type (ADD_HEAP),
		i (0),
		key (random(min_key, max_key)) {}

	CEvent (size_t n, int min_key, int max_key, CTypesChances generator = CTypesChances()):
		i (random(n-1)),
		key (random(min_key, max_key)) {
		type = generator.get_rand();
	}

	template<class THeap>
	int do_it(vector<IHeap*>& heaps) const;

	string log() const;
};

//implementation
//----------------------------------------------------------------------------------------

string CEvent::log() const {
	string result = to_string(type) + " ";

	if (type == INSERT) {
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
	if (type == ADD_HEAP) {
		heaps.push_back(new THeap());
	}
	if (type == INSERT) {
		heaps[i]->insert(key);
	}
	if (type == GET_MIN) {
		return heaps[i]->get_min();
	}
	if (type == POP_MIN) {
		heaps[i]->pop_min();
	}
	if (type == MELD) {
		heaps[i]->meld(*(heaps.back()));
		heaps.pop_back();
	}
	if (type == SIZE) {
		return (int)heaps[i]->size();
	}
	return 0;
}

//######################################################################

class FIsCorrectNormal {
public:
	bool operator()(CEvent event, vector<size_t>& sizes) {
		if (event.type == MELD && event.i == (int)(sizes.size() - 1))
			return false;
		if ((event.type == POP_MIN|| event.type == GET_MIN) && sizes[event.i] == 0)
			return false;

		if (event.type == ADD_HEAP) 
			sizes.push_back(0);
		if (event.type == INSERT) 
			++sizes[event.i];
		if (event.type == POP_MIN) 
			--sizes[event.i];
		if (event.type == MELD) {
			sizes[event.i] += sizes.back();
			sizes.pop_back();
		}
		return true;
	}
};

class FIsCorrectBigHeaps {
private:
	size_t max_size_ = 10;
public:
	bool operator()(CEvent event, vector<size_t>& sizes) {
		FIsCorrectNormal base;
		if (event.type == 0 && sizes.size() >= max_size_) {
			return false;
		}
		else {
			return base(event, sizes);
		}
	}
};

template<class TIsCorrect>
vector<CEvent> gen_random_events(size_t size, size_t start_size, int min_key, 
				int max_key, CTypesChances random_type = CTypesChances()) {

	vector<CEvent> events;
	vector<size_t> sizes;
	TIsCorrect is_correct;
	
	while (events.size() < start_size) {
		CEvent new_event = CEvent(min_key, max_key);
		if (is_correct(new_event, sizes))
			events.push_back(new_event);
	}

	while (events.size() < size) {
		CEvent new_event = CEvent(sizes.size(), min_key, max_key, random_type);
		if (is_correct(new_event, sizes)) 
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
		return log(last_test_);
	}
	string log(int x) const {
		string result;
		for (int i = 0; i <= x; ++i)
			result += events_[i].log();
		return result;

	}
};

//Correct test
//#################################################################################

template<class THeap, class TSafetyHeap>
bool correct_test(const vector<CEvent>& events) {
	CTester<THeap> tester(events);
	CTester<TSafetyHeap> true_tester(events);

	for (size_t i = 0; i < events.size(); ++i) {
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
	for (int i = 0; i < 1000; ++i) {
		if (!correct_test<THeap, TSafetyHeap>(
					gen_random_events<FIsCorrectNormal>(20, 1, -10, 10))) {
			cout << "Error on test " << i << "\n";
		}
	}

	for (int i = 0; i < 10; ++i) {
		if (!correct_test<THeap, TSafetyHeap>(
					gen_random_events<FIsCorrectBigHeaps>(100, 1, -10, 10))) {
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

TEST (time, random) {
	vector<CEvent> events = gen_random_events<FIsCorrectNormal>(1000000, 1, -10, 10);
	cout << "Binomial heaps: ";
	time_test<CBinomialHeap>(events);//2.0

	cout << "Leftest heaps: ";
	time_test<CLeftestHeap>(events);//0.6

	cout << "Splay heaps: ";
	time_test<CSplayHeap>(events);//0.35
}

TEST(time, big_heaps) {
	CTypesChances chances;
	chances.change(INSERT, 2.0);
	vector<CEvent> events = gen_random_events<FIsCorrectBigHeaps>(100000, 1, -10, 10);
	cout << "Binomial heaps: ";
	time_test<CBinomialHeap>(events);//0.2

	cout << "Leftest heaps: ";
	time_test<CLeftestHeap>(events);//0.3

	cout << "Splay heaps: ";
	time_test<CSplayHeap>(events);//0.035
}

}//test
