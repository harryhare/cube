#include "SolveCube2.h"
#include "Cube.h"
#include "Permutation.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include <deque>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>


SolveCube2 *SolveCube2::instance_=new SolveCube2;
int SolveCube2::random_style_=1;//0 - one unchang block,1 - no unchange block


static char basic_motion_permutation[3][8]=
{
	{1,2,3,4,6,7,8,5},
	{1,3,7,4,5,2,6,8},
	{1,2,4,8,5,6,3,7}
};

static char basic_motion_twist[3][8]=
{
	{0,0,0,0,0,0,0,0},
	{0,1,2,0,0,2,1,0},
	{0,0,1,2,0,0,2,1}
};

static int motion_translation[][3]= //{axis,id,times}
{
	{3,0,1},{3,0,2},{3,0,-1},
	{2,0,1},{2,0,2},{2,0,-1},
	{1,0,1},{3,0,2},{1,0,-1}
};

void SolveCube2::Init()
{
	Permutation::Init();
	make_table_corner();
	make_table_motion();
	make_table_transport();
	make_table_prun();
	test();
}

void SolveCube2::make_table_motion()
{
	int n;
	n=8;
	for(int i=0;i<3;i++)
	{
		int j=i*3;
		for(int k=0;k<8;k++)
		{
			table_motion_permutation_[j][k]=basic_motion_permutation[i][k]-1;
		}
		char *temp;
		temp=multiply_permutation(table_motion_permutation_[j],table_motion_permutation_[j],n);
		memcpy(table_motion_permutation_[j+1],temp,n);
		temp=multiply_permutation(table_motion_permutation_[j+1],table_motion_permutation_[j],n);
		memcpy(table_motion_permutation_[j+2],temp,n);
	}
	for(int i=0;i<3;i++)
	{
		int j=i*3;
		memcpy(table_motion_twist_[j],basic_motion_twist[i],n);
		memcpy(table_motion_twist_[j+1],multiply_twist(table_motion_twist_[j],  table_motion_twist_[j],table_motion_permutation_[j],n,3),n);
		memcpy(table_motion_twist_[j+2],multiply_twist(table_motion_twist_[j+1],table_motion_twist_[j],table_motion_permutation_[j],n,3),n);
	}
}

void SolveCube2::make_table_transport()
{
	int n;
	n=5040;
	for(int i=0;i<n;i++)
	{
		char *temp=permutation2int_inverse(i);
		for(int j=0;j<9;j++)
		{
			char *temp2=multiply_permutation(temp,table_motion_permutation_[j],8);
			table_transport_corner_[i][j]=permutation2int(temp2);
		}
	}
	n=729;
	for(int i=0;i<n;i++)
	{
		char *temp=twist2int_inverse(i);
		for(int j=0;j<9;j++)
		{
			char *temp2=multiply_twist(temp,table_motion_twist_[j],table_motion_permutation_[j],8,3);
			table_transport_twist_[i][j]=twist2int(temp2);
		}
	}
}

void SolveCube2::make_table_prun_template(int n1,int n2, int table1[][9], int table2[][9],char *table_deep,char*table_pre_move)
{
/*
	if(load_table("cube2_depth.dat",table_deep,n1*n2))
	{
		int max_depth=0;
		for(int i=0;i<n1*n2;i++)
		{
			if(table_deep[i]>max_depth)
			{
				max_depth=table_deep[i];
			}
		}
		std::cout<<"cube 2 max depth:"<<max_depth<<std::endl;
		if(load_table("cube2_pre_move.dat",table_pre_move,n1*n2))
		{
			return;
		}
	}
	*/
	if(load_table("cube2_pre_move.dat",table_pre_move,n1*n2))
	{
		return;
	}
	std::deque<int> temp;
	//temp.reserve(n1*n2);
	int count;
	int x,x1,x2;
	int y,y1,y2;
	int deep;
	memset(table_deep,-1,n1*n2);
	table_deep[0]=0;
	table_pre_move[0]=-1;
	count=1;
	temp.push_back(0);
	while(temp.empty()==false)
	{
		x=temp.front();
		temp.pop_front();
		x1= x/n2;
		x2= x%n2;
		deep= table_deep[x];
		for(int i=0;i<9;i++)
		{
			y1=table1[x1][i];
			y2=table2[x2][i];
			y=y1*n2+y2;
			if(table_deep[y]==-1)
			{
				temp.push_back(y);
				table_deep[y]=deep+1;
				table_pre_move[y]=i;
				count++;
			}
		}
	}
	assert(count==n1*n2);
	//save_table("cube2_depth.dat",table_deep,n1*n2);
	save_table("cube2_pre_move.dat",table_pre_move,n1*n2);
}


void SolveCube2::make_table_prun()
{
	make_table_prun_template(729,5040,table_transport_twist_,table_transport_corner_,table_trun_corner_twist_,table_pre_move);
}





void SolveCube2::GetColorData()
{
	int corner_color[8];
	int c0,c1,c2;
	for(int i=0;i<8;i++)
	{
		c0=*corner_pointer_[i][0];
		c1=*corner_pointer_[i][1];
		c2=*corner_pointer_[i][2];
		assert(c0<32);
		assert(c1<32);
		assert(c2<32);
		corner_color[i]=((1<<c0)|(1<<c1)|(1<<c2));
	}

	table_color_[0]=*corner_pointer_[0][0];
	table_color_[1]=*corner_pointer_[0][1];
	table_color_[2]=*corner_pointer_[0][2];
	int target1=table_color_[1];
	int target2=table_color_[2];
	table_color_[3]=find_opposite_color(corner_color,target1,target2);
	target1=table_color_[0];
	target2=table_color_[2];
	table_color_[4]=find_opposite_color(corner_color,target1,target2);
	target1=table_color_[0];
	target2=table_color_[1];
	table_color_[5]=find_opposite_color(corner_color,target1,target2);

	for(int i=0;i<6;i++)
	{
		table_color_inverse_[table_color_[i]]=i;
	}
}

void SolveCube2::GetData(Cube *cube)//kind of color less then 32
{
	assert(cube->n()==2);
	
	fill_corner_pointer(cube);
	GetColorData();
	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=get_corner_id_from_cube(*corner_pointer_[i][0],*corner_pointer_[i][1],*corner_pointer_[i][2]);
	}
	for(int i=0;i<8;i++)
	{
		corner_twist_[i]=get_corner_twist_from_cube(*corner_pointer_[i][0],*corner_pointer_[i][1],*corner_pointer_[i][2]);
	}
	
	PrintState();
	assert(solvable());
}
void SolveCube2::PrintState()
{
	if(enable_print_state_==false)
	{
		return;
	}
	std::cout<<"corner = {";
	for(int i=0;i<7;i++)
	{
		std::cout<<(int)corner_permutation_[i]+1<<",";	
	}
	std::cout<<(int)corner_permutation_[7]+1<<"}"<<std::endl;

	std::cout<<"corner_twist_ = {";
	for(int i=0;i<7;i++)
	{
		std::cout<<(int)corner_twist_[i]<<",";	
	}
	std::cout<<(int)corner_twist_[7]<<"}"<<std::endl;
}

void SolveCube2::MakeRandomCubeCore()
{
	//assert(RAND_MAX>10000);
	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=i;
	}
	int twist;
	if(random_style_==0)
	{
		std::random_shuffle(corner_permutation_+1,corner_permutation_+8);
		twist=rand()%729;
	}
	else
	{
		assert(random_style_==1);
		std::random_shuffle(corner_permutation_,corner_permutation_+8);
		twist=rand()%2187;
	}
	char * temp;
	temp=twist2int_inverse(twist);
	memcpy(corner_twist_,temp,8);
}
void SolveCube2::MakeRandomCube(Cube *cube)
{
	
	//step 1
	MakeRandomCubeCore();
	PrintState();
	assert(solvable());	

	//step2
	assert(cube->n()==2);
	fill_corner_pointer(cube);
	GetColorData();
	corner2cube();
}

int SolveCube2::permutation2int(char*a)
{
	char b[8];
	memcpy(b,a,8);
	return Permutation::RankPermutation(b,8);
}
char* SolveCube2::permutation2int_inverse(int n)
{
	static char a[8];
	Permutation::UnRankPermutation(n,8,a);
	return a;
}

int  SolveCube2::twist2int(char* a)
{
	int x=0;
	for(int i=0;i<7;i++)
	{
		x=(x*3)+a[i];
	}
	return x;
}
char* SolveCube2::twist2int_inverse(int n)
{
	assert(n<2187);
	static char a[8];
	int sum=0;
	for(int i=6;i>=0;i--)
	{
		a[i]= n%3;
		n   = n/3;
		sum+=a[i];
	}
	a[7] = (3-(sum%3))%3;
	return a;	
}

bool SolveCube2::solvable()
{
	bool b1=Permutation::IsPermutation(corner_permutation_,8) ;
	bool b2=valid_twist();
	return(b1&&b2);
}

void SolveCube2::SolveCore()
{
	int permutation=permutation2int(corner_permutation_);
	int twist=twist2int(corner_twist_);
	Search(permutation,twist);
	assert(solve_length_<=20);
}
void SolveCube2::Solve()
{
	SolveCore();
	//verify
	for(int i=0;i<solve_length_;i++)
	{
		TakeMotion(corner_permutation_,corner_twist_,solve_[i]);
	}
	assert(IsOrigin());

	//output
	std::cout<<"solution:";
	for(int i=0;i<solve_length_;i++)
	{
		std::cout<<(int)solve_[i]<<",";
	}
	std::cout<<std::endl;

	//out,to single move
	int j=0;
	for(int i=0;i<solve_length_;i++)
	{
		if(solve_[i]%3==1)
		{
			solution_single_move_[j]=solve_[i]-1;
			solution_single_move_[j+1]=solve_[i]-1;
			j+=2;
		}
		else
		{
			solution_single_move_[j]=solve_[i];
			j++;
		}
	}
	solution_single_length_=j;
}

void SolveCube2::Search(int permutation, int twist)
{
	int t_p=twist*5040+permutation;
	solve_length_=0;
	while(t_p)
	{
		//int x=table_trun_corner_twist_[t_p];//useless
		int pre_move=table_pre_move[t_p];
		int reverse_pre_move=pre_move/3*3+2-(pre_move%3);
		solve_[solve_length_]=reverse_pre_move;
		solve_length_++;
		permutation=table_transport_corner_[permutation][reverse_pre_move];
		twist=table_transport_twist_[twist][reverse_pre_move];
		t_p=twist*5040+permutation;
	}
}

bool SolveCube2::IsOrigin()
{
	int corner=permutation2int(corner_permutation_);
	int twist =twist2int(corner_twist_);
	assert(corner==0);
	assert(twist==0);
	return !(corner||twist);
}

void SolveCube2::TakeMotion(char *permutation,char * twist,int move)
{
	char *temp;
	temp=multiply_permutation(permutation,table_motion_permutation_[move],8);
	memcpy(permutation,temp,8);
	temp=multiply_twist(twist,table_motion_twist_[move],table_motion_permutation_[move],8,3);
	memcpy(twist,temp,8);
}


void SolveCube2::test()
{

}