#include "pch.h"
#include <vector>

const bool USE_GOOGLE_TEST = false;

int google_test(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
extern class CSplayHeap;
void manual_test() {
	CSplayHeap a;
	a.insert(7);
	a.insert(7);
	a.pop_min();
	a.insert(1);
	a.insert(4);
	a.insert(4);
	a.insert(2);
	a.size();
}

int main(int argc, char **argv) {
	if (USE_GOOGLE_TEST) {
		int result = google_test(argc, argv);
		system("pause");
		return result;
	}
	else {
		manual_test();
	}
}