#include "SolveCube3.h"
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

SolveCube3 * SolveCube3::instance_=new SolveCube3;
char SolveCube3::table_motion_corner[18][8];
char SolveCube3::table_motion_edge[18][12];
char SolveCube3::table_motion_twist[18][8];
char SolveCube3::table_motion_flip[18][12];

int  SolveCube3::table_transport_corner[40320][18];//8!
int  SolveCube3::table_transport_twist[2187][18];//3^7
int  SolveCube3::table_transport_flip[2048][18];//2^11
int	 SolveCube3::table_transport_edge8[40320][18];//8!
int  SolveCube3::table_transport_edge4[24][18];//4!
int  SolveCube3::table_transport_slice[495][18];//12*11*10*9/4!

char SolveCube3::table_trun_slice_flip[495*2048];
char SolveCube3::table_trun_slice_twist[495*2187];
char SolveCube3::table_trun_edge4_edge8[24*40320];
char SolveCube3::table_trun_edge4_corner[24*40320];

static char basic_motion_corner[6][8] = 
{
	{4,1,2,3,5,6,7,8},
	{5,2,3,1,8,6,7,4},
	{2,6,3,4,1,5,7,8},
	{1,2,3,4,6,7,8,5},
	{1,3,7,4,5,2,6,8},
	{1,2,4,8,5,6,3,7}
};
static char basic_motion_edge[6][12] =
{
	{4,1,2,3,5,6,7,8,9,10,11,12},
	{9,2,3,4,12,6,7,8,5,10,11,1},
	{1,10,3,4,5,9,7,8,2,6,11,12},
	{1,2,3,4,6,7,8,5,9,10,11,12},
	{1,2,11,4,5,6,10,8,9,3,7,12},
	{1,2,3,12,5,6,7,11,9,10,4,8},  
};
static char basic_motion_twist[6][8] =
{
	{0,0,0,0,0,0,0,0},
	{2,0,0,1,1,0,0,2},
	{1,2,0,0,2,1,0,0},
	{0,0,0,0,0,0,0,0},
	{0,1,2,0,0,2,1,0},
	{0,0,1,2,0,0,2,1}
};
static char basic_motion_flip[6][12] =
{
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,0,0,0,1,0,0,1,1,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,0,0,0,1,0,0,1,1}   
};
static int motion_translation[][3] = //{axis,id,times}
{
	{3,2,-1},{3,2,2},{3,2,1},
	{2,2,-1},{2,2,2},{2,2,1},
	{1,2,-1},{3,2,2},{1,2,1},
	{3,0,1},{3,0,2},{3,0,-1},
	{2,0,1},{2,0,2},{2,0,-1},
	{1,0,1},{3,0,2},{1,0,-1}
};

void SolveCube3::Init()
{
	Permutation::Init();
	make_table_corner();
	make_table_edge();
	make_table_motion();
	make_table_transport();
	make_table_prun();
	test();
}





void SolveCube3::make_table_motion()
{
	int n;
	n=8;
	for(int i=0;i<6;i++)
	{
		int j=i*3;
		//memcpy(table_motion_corner[j],basic_motion_corner[i],n);
		for(int k=0;k<8;k++)
		{
			table_motion_corner[j][k]=basic_motion_corner[i][k]-1;
		}
		memcpy(table_motion_corner[j+1],multiply_permutation(table_motion_corner[j],table_motion_corner[j],n),n);
		memcpy(table_motion_corner[j+2],multiply_permutation(table_motion_corner[j+1],table_motion_corner[j],n),n);
	}
	n=12;
	for(int i=0;i<6;i++)
	{
		int j=i*3;
		for(int k=0;k<12;k++)
		{
			table_motion_edge[j][k]=basic_motion_edge[i][k]-1;
		}
		//memcpy(table_motion_edge[j],basic_motion_edge[i],n);
		memcpy(table_motion_edge[j+1],multiply_permutation(table_motion_edge[j],table_motion_edge[j],n),n);
		memcpy(table_motion_edge[j+2],multiply_permutation(table_motion_edge[j+1],table_motion_edge[j],n),n);
	}

	n=8;
	for(int i=0;i<6;i++)
	{
		int j=i*3;
		memcpy(table_motion_twist[j],basic_motion_twist[i],n);
		memcpy(table_motion_twist[j+1],multiply_twist(table_motion_twist[j],  table_motion_twist[j],table_motion_corner[j],n,3),n);
		memcpy(table_motion_twist[j+2],multiply_twist(table_motion_twist[j+1],table_motion_twist[j],table_motion_corner[j],n,3),n);
	}

	n=12;
	for(int i=0;i<6;i++)
	{
		int j=i*3;
		memcpy(table_motion_flip[j],basic_motion_flip[i],n);
		memcpy(table_motion_flip[j+1],multiply_twist(table_motion_flip[j],  table_motion_flip[j],table_motion_edge[j],n,2),n);
		memcpy(table_motion_flip[j+2],multiply_twist(table_motion_flip[j+1],table_motion_flip[j],table_motion_edge[j],n,2),n);
	}
}

void SolveCube3::make_table_transport()
{
	int n;
	n=40320;
	for(int i=0;i<n;i++)
	{
		char *temp=corner2int_inverse(i);
		for(int j=0;j<18;j++)
		{
			char *temp2=multiply_permutation(temp,table_motion_corner[j],8);
			table_transport_corner[i][j]=corner2int(temp2);
		}
		
	}
	n=2187;
	for(int i=0;i<n;i++)
	{
		char *temp=twist2int_inverse(i);
		for(int j=0;j<18;j++)
		{
			char* temp2=multiply_twist(temp,table_motion_twist[j],table_motion_corner[j],8,3);
			table_transport_twist[i][j]=twist2int(temp2);
		}
	}
	n=2048;
	for(int i=0;i<n;i++)
	{
		char *temp=flip2int_inverse(i);
		for(int j=0;j<18;j++)
		{
			char *temp2=multiply_twist(temp,table_motion_flip[j],table_motion_edge[j],12,2);
			table_transport_flip[i][j]=flip2int(temp2);
		}
	}
	n=40320;
	for(int i=0;i<n;i++)
	{
		char *temp=edge8int_inverse(i);
		for(int j=0;j<18;j++)
		{
			if(changed_motion(j))
			{
				continue;
			}
			char *temp2=multiply_permutation(temp,table_motion_edge[j],8);
			table_transport_edge8[i][j]=edge8int(temp2);
		}
	}
	n=24;
	for(int i=0;i<n;i++)
	{
		char *temp=edge4int_inverse(i);
		for(int j=0;j<18;j++)
		{
			if(changed_motion(j))
			{
				continue;
			}
			char *temp2=multiply_permutation(temp,table_motion_edge[j],12);
			table_transport_edge4[i][j]=edge4int(temp2);
		}
	}
	n=495;
	for(int i=0;i<n;i++)
	{
		char *temp=slice2int_inverse(i);
		for(int j=0;j<18;j++)
		{
			char *temp2=multiply_permutation(temp,table_motion_edge[j],12);
			table_transport_slice[i][j]=slice2int(temp2);
		}
	}
}
void SolveCube3::make_table_prun_template(char*name,int n1,int n2, int table1[][18], int table2[][18],char *table_deep,bool skip)
{
	if(load_table(name,table_deep,n1*n2))
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
	count=1;
	temp.push_back(0);
	while(temp.empty()==false)
	{
		x=temp.front();
		temp.pop_front();
		x1= x/n2;
		x2= x%n2;
		deep= table_deep[x];
		for(int i=0;i<18;i++)
		{
			if(skip&&changed_motion(i))
			{
				continue;
			}
			y1=table1[x1][i];
			y2=table2[x2][i];
			y=y1*n2+y2;
			if(table_deep[y]==-1)
			{
				temp.push_back(y);
				table_deep[y]=deep+1;
				count++;
			}
		}
	}
	assert(count==n1*n2);
	save_table(name,table_deep,n1*n2);
}

void SolveCube3::make_table_prun()
{
	make_table_prun_template("cube3_trun_slice_flip.dat",495,2048,table_transport_slice,table_transport_flip,table_trun_slice_flip);
	make_table_prun_template("cube3_trun_slice_twist.dat",495,2187,table_transport_slice,table_transport_twist,table_trun_slice_twist);
	make_table_prun_template("cube3_trun_edge4_edge8.dat",24,40320,table_transport_edge4,table_transport_edge8,table_trun_edge4_edge8,true);
	make_table_prun_template("cube3_trun_edge4_corner.dat",24,40320,table_transport_edge4,table_transport_corner,table_trun_edge4_corner,true);
}




void SolveCube3::GetColorData(Cube *cube)
{
	table_color_[0]=cube->top()[4];
	table_color_[1]=cube->right()[4];
	table_color_[2]=cube->front()[4];
	table_color_[3]=cube->bottom()[4];
	table_color_[4]=cube->left()[4];
	table_color_[5]=cube->back()[4];

	for(int i=0;i<6;i++)
	{
		table_color_inverse_[table_color_[i]]=i;
	}
}

void SolveCube3::GetData(char *corner_permutation,char*corner_twist,char* edge_permutation,char* edge_flip)
{
	memcpy(corner_permutation_,corner_permutation,8);
	memcpy(corner_twist_,corner_twist,8);
	memcpy(edge_permutation_,edge_permutation,12);
	memcpy(edge_flip_,edge_flip,12);
}

void SolveCube3::GetData(Cube * cube)
{
	assert(cube->n()==3);
	GetColorData(cube);
/*
	corner_permutation_[0]=get_corner_id_from_cube(cube->top()[8]   ,cube->right()[8],cube->front()[8]);
	corner_permutation_[1]=get_corner_id_from_cube(cube->top()[6]   ,cube->front()[2],cube->left()[8]);
	corner_permutation_[2]=get_corner_id_from_cube(cube->top()[0]   ,cube->left()[6],cube->back()[2]);
	corner_permutation_[3]=get_corner_id_from_cube(cube->top()[2]   ,cube->back()[8],cube->right()[6]);
	corner_permutation_[4]=get_corner_id_from_cube(cube->bottom()[8],cube->front()[6],cube->right()[2]);
	corner_permutation_[5]=get_corner_id_from_cube(cube->bottom()[6],cube->left()[2],cube->front()[0]);
	corner_permutation_[6]=get_corner_id_from_cube(cube->bottom()[0],cube->back()[0],cube->left()[0]);
	corner_permutation_[7]=get_corner_id_from_cube(cube->bottom()[2],cube->right()[0],cube->back()[6]);

	corner_twist_[0]=get_corner_twist_from_cube(cube->top()[8]   ,cube->right()[8],cube->front()[8]);
	corner_twist_[1]=get_corner_twist_from_cube(cube->top()[6]   ,cube->front()[2],cube->left()[8]);
	corner_twist_[2]=get_corner_twist_from_cube(cube->top()[0]   ,cube->left()[6],cube->back()[2]);
	corner_twist_[3]=get_corner_twist_from_cube(cube->top()[2]   ,cube->back()[8],cube->right()[6]);
	corner_twist_[4]=get_corner_twist_from_cube(cube->bottom()[8],cube->front()[6],cube->right()[2]);
	corner_twist_[5]=get_corner_twist_from_cube(cube->bottom()[6],cube->left()[2],cube->front()[0]);
	corner_twist_[6]=get_corner_twist_from_cube(cube->bottom()[0],cube->back()[0],cube->left()[0]);
	corner_twist_[7]=get_corner_twist_from_cube(cube->bottom()[2],cube->right()[0],cube->back()[6]);

	edge_permutation_[0]=get_edge_id_from_cube(cube->top()[5], cube->right()[7]);
	edge_permutation_[1]=get_edge_id_from_cube(cube->top()[7], cube->front()[5]);
	edge_permutation_[2]=get_edge_id_from_cube(cube->top()[3], cube->left()[7]);
	edge_permutation_[3]=get_edge_id_from_cube(cube->top()[1], cube->back()[5]);
	edge_permutation_[4]=get_edge_id_from_cube(cube->bottom()[5], cube->right()[1]);
	edge_permutation_[5]=get_edge_id_from_cube(cube->bottom()[7], cube->front()[3]);
	edge_permutation_[6]=get_edge_id_from_cube(cube->bottom()[3], cube->left()[1]);
	edge_permutation_[7]=get_edge_id_from_cube(cube->bottom()[1], cube->back()[3]);
	edge_permutation_[8]=get_edge_id_from_cube(cube->front()[7], cube->right()[5]);
	edge_permutation_[9]=get_edge_id_from_cube(cube->front()[1], cube->left()[5]);
	edge_permutation_[10]=get_edge_id_from_cube(cube->back()[1], cube->left()[3]);
	edge_permutation_[11]=get_edge_id_from_cube(cube->back()[7], cube->right()[3]);

	edge_flip_[0]=get_edge_flip_from_cube(cube->top()[5], cube->right()[7]);
	edge_flip_[1]=get_edge_flip_from_cube(cube->top()[7], cube->front()[5]);
	edge_flip_[2]=get_edge_flip_from_cube(cube->top()[3], cube->left()[7]);
	edge_flip_[3]=get_edge_flip_from_cube(cube->top()[1], cube->back()[5]);
	edge_flip_[4]=get_edge_flip_from_cube(cube->bottom()[5], cube->right()[1]);
	edge_flip_[5]=get_edge_flip_from_cube(cube->bottom()[7], cube->front()[3]);
	edge_flip_[6]=get_edge_flip_from_cube(cube->bottom()[3], cube->left()[1]);
	edge_flip_[7]=get_edge_flip_from_cube(cube->bottom()[1], cube->back()[3]);
	edge_flip_[8]=get_edge_flip_from_cube(cube->front()[7], cube->right()[5]);
	edge_flip_[9]=get_edge_flip_from_cube(cube->front()[1], cube->left()[5]);
	edge_flip_[10]=get_edge_flip_from_cube(cube->back()[1], cube->left()[3]);
	edge_flip_[11]=get_edge_flip_from_cube(cube->back()[7], cube->right()[3]);
	*/
	int* corners[8][3];
	int* edges[12][2];
	corners[0][0]=&(cube->top()[8])		;corners[0][1]=&(cube->right()[8]);	corners[0][2]=&(cube->front()[8]);
	corners[1][0]=&(cube->top()[6])		;corners[1][1]=&(cube->front()[2]);	corners[1][2]=&(cube->left()[8]);
	corners[2][0]=&(cube->top()[0])		;corners[2][1]=&(cube->left()[6]);	corners[2][2]=&(cube->back()[2]);
	corners[3][0]=&(cube->top()[2])		;corners[3][1]=&(cube->back()[8]);	corners[3][2]=&(cube->right()[6]);
	corners[4][0]=&(cube->bottom()[8])	;corners[4][1]=&(cube->front()[6]);	corners[4][2]=&(cube->right()[2]);
	corners[5][0]=&(cube->bottom()[6])	;corners[5][1]=&(cube->left()[2]);	corners[5][2]=&(cube->front()[0]);
	corners[6][0]=&(cube->bottom()[0])	;corners[6][1]=&(cube->back()[0]);	corners[6][2]=&(cube->left()[0]);
	corners[7][0]=&(cube->bottom()[2])	;corners[7][1]=&(cube->right()[0]);	corners[7][2]=&(cube->back()[6]);

	edges[0][0]=&(cube->top()[5]);		edges[0][1]=&(cube->right()[7]);
	edges[1][0]=&(cube->top()[7]);		edges[1][1]=&(cube->front()[5]);
	edges[2][0]=&(cube->top()[3]);		edges[2][1]=&(cube->left()[7]);
	edges[3][0]=&(cube->top()[1]);		edges[3][1]=&(cube->back()[5]);
	edges[4][0]=&(cube->bottom()[5]);	edges[4][1]=&(cube->right()[1]);
	edges[5][0]=&(cube->bottom()[7]);	edges[5][1]=&(cube->front()[3]);
	edges[6][0]=&(cube->bottom()[3]);	edges[6][1]=&(cube->left()[1]);
	edges[7][0]=&(cube->bottom()[1]);	edges[7][1]=&(cube->back()[3]);
	edges[8][0]=&(cube->front()[7]);	edges[8][1]=&(cube->right()[5]);
	edges[9][0]=&(cube->front()[1]);	edges[9][1]=&(cube->left()[5]);
	edges[10][0]=&(cube->back()[1]);	edges[10][1]=&(cube->left()[3]);
	edges[11][0]=&(cube->back()[7]);	edges[11][1]=&(cube->right()[3]);

	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=get_corner_id_from_cube(*corners[i][0],*corners[i][1],*corners[i][2]);
	}
	for(int i=0;i<8;i++)
	{
		corner_twist_[i]=get_corner_twist_from_cube(*corners[i][0],*corners[i][1],*corners[i][2]);
	}
	for(int i=0;i<12;i++)
	{
		edge_permutation_[i]=get_edge_id_from_cube(*edges[i][0], *edges[i][1]);
	}
	for(int i=0;i<12;i++)
	{
		edge_flip_[i]=get_edge_flip_from_cube(*edges[i][0], *edges[i][1]);
	}
	PrintState();
	assert(solvable());
}
void SolveCube3::PrintState()
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
	std::cout<<"edge = {";
	for(int i=0;i<11;i++)
	{
		std::cout<<(int)edge_permutation_[i]+1<<",";	
	}
	std::cout<<(int)edge_permutation_[11]+1<<"}"<<std::endl;
	std::cout<<"corner_twist_ = {";
	for(int i=0;i<7;i++)
	{
		std::cout<<(int)corner_twist_[i]<<",";	
	}
	std::cout<<(int)corner_twist_[7]<<"}"<<std::endl;
	std::cout<<"edge_flip_ = {";
	for(int i=0;i<11;i++)
	{
		std::cout<<(int)edge_flip_[i]<<",";	
	}
	std::cout<<(int)edge_flip_[11]<<"}"<<std::endl;
}

void SolveCube3::MakeRandomCubeCore()
{
	//assert(RAND_MAX>10000);
	/*
	//FOR TEST
	for(int i=0;i<12;i++)
	{
		edge_permutation_[i]=basic_motion_edge[0][i]-1;
	}
	for(int i=0;i<12;i++)
	{
		edge_flip_[i]=basic_motion_flip[0][i];
	}
	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=basic_motion_corner[0][i]-1;
	}
	for(int i=0;i<8;i++)
	{
		corner_twist_[i]=basic_motion_twist[0][i];
	}
	PrintState();
	*/
	make_random_corner();
	make_random_middle_edge();
}

void SolveCube3::MakeRandomCube(Cube *cube)
{
	//step 1
	MakeRandomCubeCore();
	PrintState();
	assert(solvable());	

	//step2
	assert(cube->n()==3);
	fill_corner_pointer(cube);
	fill_midedge_pointer(cube);
	GetColorData(cube);
	corner2cube();
	midedge2cube();

}

int	 SolveCube3::slice2int(char* a)
{
	char b[4];
	for(int i=0,j=0;i<12;i++)
	{
		if(a[i]>=8)
		{
			b[j]=i;
			j++;
		}
	}
	int n=Permutation::RankSubset(b,4,12);
	return 494-n;
}
char* SolveCube3::slice2int_inverse(int n)
{
	static char a[12];
	char b[4];
	n=494-n;
	Permutation::UnRankSubset(n,4,12,b);
	for(int i=0;i<12;i++)
	{
		a[i]=-1;
	}
	for(int i=0;i<4;i++)
	{
		a[b[i]]=8+i;
	}
	for(int i=0,j=0;i<12;i++)
	{
		if(a[i]==-1)
		{
			a[i]=j;
			j++;
		}
	}
	return a;
}

int  SolveCube3::edge8int(char* a)
{
	char b[8];
	memcpy(b,a,8);
	return Permutation::RankPermutation(b,8);
}
char* SolveCube3::edge8int_inverse(int n)
{
	static char a[12];
	Permutation::UnRankPermutation(n,8,a);
	for(int i=8;i<12;i++)
	{
		a[i]=i;
	}
	return a;
}
int  SolveCube3::edge4int(char* a)
{
	char b[4];
	for(int i=0;i<4;i++)
	{
		b[i]=a[i+8]-8;
	}
	return Permutation::RankPermutation(b,4);
}
char* SolveCube3::edge4int_inverse(int n)
{
	static char a[12];
	for(int i=0;i<8;i++)
	{
		a[i]=i;
	}
	Permutation::UnRankPermutation(n,4,a+8);
	for(int i=0;i<4;i++)
	{
		a[i+8]+=8;
	}
	return a;
}





bool SolveCube3::solvable()
{
	bool b1=Permutation::IsPermutation(corner_permutation_,8) ;
	bool b2=Permutation::IsPermutation(edge_permutation_,12);
	bool b3=(Permutation::Signature(corner_permutation_,8)==Permutation::Signature(edge_permutation_,12));
	bool b4=valid_flip();
	bool b5=valid_twist();
	assert(b1);
	assert(b2);
	assert(b3);
	assert(b4);
	assert(b5);
	return(b1&&b2&&b3&&b4&&b5);
}

void SolveCube3::SolveCore()
{
	int slice=slice2int(edge_permutation_);
	int twist=twist2int(corner_twist_);
	int flip =flip2int(edge_flip_);
	//int edge8=edge8int(edge_permutation_);
	//int edge4=edge4int(edge_permutation_);

	if(slice==0 && twist==0 && flip==0)
	{
		solve1_length_=0;
		StartSeach2();
	}
	else
	{
		int min_deep=100;//need to be big enough
		for(int i=0;i<18;i++)
		{
			int s=table_transport_slice[slice][i];
			int t=table_transport_twist[twist][i];
			int f=table_transport_flip[flip][i];
			int s_f=s*2048+f;
			int s_t=s*2187+t;
			int max=table_trun_slice_flip[s_f];
			if(table_trun_slice_twist[s_t]>max)
			{
				max=table_trun_slice_twist[s_t];
			}
			if(max<min_deep)
			{
				min_deep=max;
			}
		}

		search1_stop_=false;
		search2_stop_=false;
		for(int i=min_deep+1; i<=20 && search1_stop_==false;i++)
		{
			Search1(slice,twist,flip,i,0);
		}
	}

}


void SolveCube3::Solve()
{
	SolveCore();

	//verify
	assert(search1_stop_);
	assert(search2_stop_);
	for(int i=0;i<solve2_length_;i++)
	{
		TakeMotion(corner_permutation_,edge_permutation_,corner_twist_,edge_flip_,solve2_[i]);
	}
	assert(IsOrigin());
	

	//output
	std::cout<<"phase1:";
	for(int i=0;i<solve1_length_;i++)
	{
		std::cout<<(int)solve1_[i]<<",";
	}
	std::cout<<std::endl;
	std::cout<<"phase2:";
	for(int i=0;i<solve2_length_;i++)
	{
		std::cout<<(int)solve2_[i]<<",";
	}
	std::cout<<std::endl;


	//out
	int j=0;
	int begin=0;
	for(int i=0;i<solve1_length_;i++)
	{
		int t=solve1_[i];
		solution[j][0]=motion_translation[t][0];
		solution[j][1]=motion_translation[t][1];
		solution[j][2]=motion_translation[t][2];
		j++;
	}
	if(solve1_length_!=0&&solve2_length_!=0)
	{
		if(solve1_[solve1_length_-1]/3==solve2_[0]/3)
		{
			int t=solve1_[solve1_length_-1]/3*3+(solve1_[solve1_length_-1]%3+solve2_[0]%3)%3;
			solution[j-1][0]=motion_translation[t][0];
			solution[j-1][1]=motion_translation[t][1];
			solution[j-1][2]=motion_translation[t][2];
			begin=1;
		}
	}
	for(int i=begin;i<solve2_length_;i++)
	{
		int t=solve2_[i];
		solution[j][0]=motion_translation[t][0];
		solution[j][1]=motion_translation[t][1];
		solution[j][2]=motion_translation[t][2];
		j++;
	}
	//out
	j=0;
	begin=0;
	for(int i=0;i<solve1_length_;i++)
	{
		solution_[j]=solve1_[i];
		j++;
	}
	if(solve1_length_!=0&&solve2_length_!=0)
	{
		if(solve1_[solve1_length_-1]/3==solve2_[0]/3)
		{
			int t=solve1_[solve1_length_-1]/3*3+(solve1_[solve1_length_-1]%3+solve2_[0]%3+1)%3;
			solution_[j-1]=t;
			begin=1;
		}
	}
	for(int i=begin;i<solve2_length_;i++)
	{
		solution_[j]=solve2_[i];
		j++;
	}
	solution_length=j;

	//out
	j=0;
	for(int i=0;i<solution_length;i++)
	{
		if(solution_[i]%3==1)
		{
			solution_single_move_[j]=solution_[i]-1;
			solution_single_move_[j+1]=solution_[i]-1;
			j+=2;
		}
		else
		{
			solution_single_move_[j]=solution_[i];
			j++;
		}
	}
	solution_single_length_=j;
}

bool SolveCube3::IsOrigin()
{
	int slice= slice2int(edge_permutation_);
	int twist= twist2int(corner_twist_);
	int flip=  flip2int(edge_flip_);
	int corner= corner2int(corner_permutation_);
	int edge8=  edge8int(edge_permutation_);
	int edge4=  edge4int(edge_permutation_);
	assert(slice==0);
	assert(twist==0);
	assert(flip==0);
	assert(corner==0);
	assert(edge8==0);
	assert(edge4==0);
	return !(slice||twist||flip||corner||edge8||edge4);
}

void SolveCube3::Search1(int slice,int twist, int flip,int max_deep, int index)
{
	for(int i=0;i<18 && search1_stop_==false;i++)
	{
		int s=table_transport_slice[slice][i];
		int t=table_transport_twist[twist][i];
		int f=table_transport_flip[flip][i];
		int s_f=s*2048+f;
		int s_t=s*2187+t;
		int deep1=table_trun_slice_flip[s_f];
		int deep2=table_trun_slice_twist[s_t];
		int max=(deep1>deep2?deep1:deep2);
		if(max==0)
		{
			solve1_[index]=i;
			solve1_length_=index+1;
			StartSeach2();
		}
		if(max<max_deep)
		{
			if((index>0 && valid_pair(i,solve1_[index-1]))||index==0)
			{
				solve1_[index]=i;
				Search1(s,t,f,max_deep-1,index+1);
			}
		}
	}
}

void SolveCube3::StartSeach2()
{
	//search1_stop_=true;
	
	for(int i=0;i<solve1_length_;i++)
	{
		TakeMotion(corner_permutation_,edge_permutation_,corner_twist_,edge_flip_,solve1_[i]);
	}
	int slice= slice2int(edge_permutation_);
	int twist= twist2int(corner_twist_);
	int flip=  flip2int(edge_flip_);
	assert(slice==0);
	assert(twist==0);
	assert(flip==0);
	int corner= corner2int(corner_permutation_);
	int edge8=  edge8int(edge_permutation_);
	int edge4=  edge4int(edge_permutation_);
	if(edge4==0 && corner==0 && edge8==0)
	{
		solve2_length_=0;
		search1_stop_=true;
		search2_stop_=true;
		return;
	}

	int min_deep=100;//need to be big enough
	for(int i=0;i<18;i++)
	{
		if(changed_motion(i))
		{
			continue;
		}
		int e4=table_transport_edge4[edge4][i];
		int c =table_transport_corner[corner][i];
		int e8=table_transport_edge8[edge8][i];
		int e4_c =e4*40320+c;
		int e4_e8=e4*40320+e8;
		int max=table_trun_edge4_corner[e4_c];
		if(table_trun_edge4_edge8[e4_e8]>max)
		{
			max=table_trun_edge4_edge8[e4_e8];
		}
		if(max<min_deep)
		{
			min_deep=max;
		}
	}

	search2_stop_=false;
	for(int i=min_deep+1; i<=20 && search2_stop_==false;i++)
	{
		Search2(edge4,corner,edge8,i,0);
	}
}

void SolveCube3::Search2(int edge4,int corner,int edge8,int max_deep, int index)
{
	for(int i=0;i<18 && search2_stop_==false;i++)
	{
		if(changed_motion(i))
		{
			continue;
		}
		int e4=table_transport_edge4[edge4][i];
		int c =table_transport_corner[corner][i];
		int e8=table_transport_edge8[edge8][i];
		int e4_c =e4*40320+c;
		int e4_e8=e4*40320+e8;
		int deep1=table_trun_edge4_corner[e4_c];
		int deep2=table_trun_edge4_edge8[e4_e8];
		int max=(deep1>deep2?deep1:deep2);
		if(max==0)
		{
			solve2_[index]=i;
			solve2_length_=index+1;
			search2_stop_=true;
			search1_stop_=true;
			return;
		}
		if(max<max_deep)
		{
			if((index>0 && valid_pair(i,solve1_[index-1]))||index==0)
			{
				solve2_[index]=i;
				Search2(e4,c,e8,max_deep-1,index+1);
			}
		}
	}
}
void SolveCube3::TakeMotion(char *p_corner, char *p_edge, char * twist, char * flip, int move)
{
	char *temp;
	temp=multiply_permutation(p_corner,table_motion_corner[move],8);
	memcpy(p_corner,temp,8);
	temp=multiply_permutation(p_edge,table_motion_edge[move],12);
	memcpy(p_edge,temp,12);
	temp=multiply_twist(twist,table_motion_twist[move],table_motion_corner[move],8,3);
	memcpy(twist,temp,8);
	temp=multiply_twist(flip,table_motion_flip[move],table_motion_edge[move],12,2);
	memcpy(flip,temp,12);
}


void SolveCube3::test()
{
	for(int i=0;i<2048;i++)
	{
		assert(flip2int(flip2int_inverse(i))==i);
	}
	for(int i=0;i<2187;i++)
	{
		assert(twist2int(twist2int_inverse(i))==i);
	}

	for(int i=0;i<24;i++)
	{
		assert(edge4int(edge4int_inverse(i))==i);
	}

	for(int i=0;i<40320;i++)
	{
		assert(edge8int(edge8int_inverse(i))==i);
	}
	for(int i=0;i<40320;i++)
	{
		assert(corner2int(corner2int_inverse(i))==i);
	}
	for(int i=0;i<495;i++)
	{
		assert(slice2int(slice2int_inverse(i))==i);
	}
}