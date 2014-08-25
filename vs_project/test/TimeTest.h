//time test

#ifndef CUBE_TIMETEST_H_
#define CUBE_TIMETEST_H_


#include "solver/SolveCube2.h"
#include "solver/SolveCube3.h"
#include "solver/SolveCubeN.h"

#include <ctime>
#include <fstream>

class TimeTest
{
public:
	TimeTest(){}
	~TimeTest(){}
	
	void TestCube2(int test_num=100);
	void TestCube3(int test_num=100);
	void TestCubeN(int n,int test_num=100);

	void Clocker();
	void clock_begin();
	void clock_end();
	void print_info(int cube_order);
private:

	time_t begin_time_;
	time_t end_time_;
	clock_t begin_clock_;
	clock_t end_clock_;

	int repeat_times_;
	double average_move_;
	double average_single_move_;

	std::ofstream out_file_;
};


#endif