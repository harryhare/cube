#include "TimeTest.h"
#include <Windows.h>
#include <iostream>




void TimeTest::clock_begin()
{
	begin_time_=time(NULL);
	begin_clock_=clock();
}
void TimeTest::clock_end()
{
	end_clock_=clock();
	end_time_=time(NULL);

}

void TimeTest::print_info(int cube_order)
{
	double time1;
	double time2;
	double average_time;
	time1=(double)(end_time_-begin_time_);
	time2=(end_clock_-begin_clock_)/(double)CLOCKS_PER_SEC;
	average_time=time2/repeat_times_*1000;
	std::cout<<"Cube "<<cube_order<<" "<<std::endl;
	std::cout<<"repeat times : "<<repeat_times_<<std::endl;
	std::cout<<"time1 : "<<time1<<"s"<<std::endl;
	std::cout<<"time2 : "<<time2<<"s"<<std::endl;
	std::cout<<"average time : "<<average_time<<"ms"<<std::endl;
	std::cout<<"average move : "<<average_move_<<std::endl;
	if(average_single_move_!=0)
	{
		std::cout<<"average single move : "<<average_single_move_<<std::endl;
	}
	std::cout<<"------------------------------------"<<std::endl;
	

	out_file_<<cube_order<<'\t'<<repeat_times_<<'\t'<<time1<<'\t'<<time2<<'\t'<<average_time<<'\t'<<average_move_<<'\t';
	if(average_single_move_!=0)
	{
		out_file_<<average_single_move_;
	}
	out_file_<<std::endl;
	CLOCKS_PER_SEC;
}

void TimeTest::Clocker()
{
	out_file_.open("statstics.txt");

	std::cout<<"------------------------------------"<<std::endl;

	TestCube2(1);
	clock_begin();
	repeat_times_=1000;
	TestCube2(repeat_times_);
	clock_end();
	print_info(2);


	TestCube3(1);
	clock_begin();
	repeat_times_=100;
	TestCube3(repeat_times_);
	clock_end();
	print_info(3);

	for(int i=1000;i<=10000;i+=1000)
	{
		TestCubeN(i,1);
		clock_begin();
		repeat_times_=5;
		TestCubeN(i,repeat_times_);
		clock_end();
		print_info(i);
	}
/*
	for(int i=4;i<30;i++)
	{
		TestCubeN(i,1);
		clock_begin();
		repeat_times_=10;
		TestCubeN(i,repeat_times_);
		clock_end();
		print_info(i);
	}

	for(int i=50;i<=500;i+=50)
	{
		TestCubeN(i,1);
		clock_begin();
		repeat_times_=5;
		TestCubeN(i,repeat_times_);
		clock_end();
		print_info(i);
	}
*/
	out_file_.close();
}

void TimeTest::TestCube2(int test_num)
{
	Cube cube(2);
	int step_count=0;

	for(int i=0;i<test_num;i++)
	{
		SolveCube2::GetInstance()->MakeRandomCube(&cube);
		SolveCube2::GetInstance()->GetData(&cube);
		SolveCube2::GetInstance()->SolveCore();
		step_count+=SolveCube2::GetInstance()->solve_length_;
	}
	average_move_=step_count/(double)test_num;
	average_single_move_=0;
}

void TimeTest::TestCube3(int test_num)
{
	int step_count=0;
	for(int i=0;i<test_num;i++)
	{
		SolveCube3::GetInstance()->MakeRandomCubeCore();
		SolveCube3::GetInstance()->SolveCore();
		step_count+=SolveCube3::GetInstance()->solve1_length_+SolveCube3::GetInstance()->solve2_length_;
	}
	average_move_=step_count/(double)test_num;
	average_single_move_=0;
}

void TimeTest::TestCubeN(int n,int test_num)
{
	int step_count1=0;
	int step_count2=0;
	for(int i=0;i<test_num;i++)
	{
		SolveCubeN::GetInstance()->MakeRandomCubeCore(n);
		SolveCubeN::GetInstance()->Solve();
		step_count1+=SolveCubeN::GetInstance()->solution_.size();
		step_count2+=SolveCubeN::GetInstance()->solution_single_.size();
	}
	average_move_=step_count1/(double)test_num;
	average_single_move_=step_count2/(double)test_num;
}



int main(int argc,char**argv)
{
	TimeTest test;
	test.Clocker();
	system("pause");
}
