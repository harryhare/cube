#include "SolveCubeN.h"
#include "SolveCube3.h"
#include "Permutation.h"
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <deque>

bool test_mode=false;
int SolveCubeN::solve_mode_=2;
static char complex_motion_face_append[48][3];
static char complex_motion_face_moves[48][8][3];
static char complex_motion_edge_append[24][2];
static char complex_motion_edge_moves[24][12][3];
static char complex_motion_edge_append_special[24][2];
static char complex_motion_edge_moves_special[24][15][3];

SolveCubeN* SolveCubeN::instance_=new SolveCubeN;
static char edge_second_face[6][4]=
{
	{5,1,2,4},
	{3,2,0,5},
	{4,0,1,3},
	{4,2,1,5},
	{5,0,2,3},
	{3,1,0,4},
};// original state


char SolveCubeN::table_edge_id2color_[24];
char SolveCubeN::table_edge_color2id_[36];// only 24 valid

static int * get_4index(int i,int j,int cube_order_)
{
	static int index[4];
	int n=cube_order_-1;
	index[0] = i*cube_order_+j;
	index[1] = j*cube_order_+(n-i);
	index[2] = (n-i)*cube_order_+(n-j);
	index[3] = (n-j)*cube_order_+i;
	return index;
}
static int * get_4index_inverse(int i,int j,int cube_order_)
{
	static int index[4];
	int n=cube_order_-1;
	index[0] = i*cube_order_+j;
	index[3] = j*cube_order_+(n-i);
	index[2] = (n-i)*cube_order_+(n-j);
	index[1] = (n-j)*cube_order_+i;
	return index;
}


void SolveCubeN::Init()
{
	Permutation::Init();

	make_table_corner();
	make_table_edge();
	make_table_edgeN();

	make_table_motion();
	make_table_transport();
	make_table_trun();
}

void SolveCubeN::test()
{
	
}

void SolveCubeN::make_table_edgeN()
{
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			table_edge_id2color_[i*4+j]=i*6+edge_second_face[i][j];
		}
	}
	for(int i=0;i<24;i++)
	{
		table_edge_color2id_[table_edge_id2color_[i]]=i;
	}
}
bool SolveCubeN::solvable()
{
	bool b1=valid_face();
	bool b2=valid_edgeN();
	bool b3=Permutation::IsPermutation(corner_permutation_,8);
	bool b4=valid_twist();
	bool b5=true;
	bool b6=true;
	if(cube_order_%2)
	{
		b5=Permutation::IsPermutation(edge_permutation_,12);
		b6=valid_flip();
	}
	return b1 && b2 && b3 && b4 && b5 && b6;
}
bool SolveCubeN::valid_face()
{
	int count[6];
	for(int i=0;i<face24_num_;i++)
	{
		memset(count,0,sizeof(count));
		assert(sizeof(count)==6*sizeof(int));
		for(int j=0;j<24;j++)
		{
			count[faces_[i][j]]++;
		}
		for(int j=0;j<6;j++)
		{
			if(count[j]!=4)
			{
				return false;
			}
		}
	}
	return true;
}
bool SolveCubeN::valid_edgeN()
{
	for(int i=0;i<edge24_num_;i++)
	{
		if(Permutation::IsPermutation(edges_[i],24)==false)
		{
			return false;
		}
	}
	return true;
}

int rand_seed=0;

void SolveCubeN::MakeRandomCubeCore(int n)
{
	//srand((unsigned int)time(NULL));
	//assert(RAND_MAX>10000);
	//step 0, prepare
	cube_order_=n;

	int x=(cube_order_-2)*(cube_order_-2)/4;
	if(face24_num_!=x)
	{
		face24_num_= x;
		if(faces_!=NULL)
		{
			free(faces_);
		}
		faces_=(Array24)malloc(face24_num_*24*sizeof(char));
	}

	x=(cube_order_-2)/2;
	if(edge24_num_!=x)
	{
		edge24_num_ = x;
		if(edges_!=NULL)
		{
			free(edges_);
		}
		edges_=(Array24)malloc(edge24_num_*24*sizeof(char));
	}

		//step 1, produce random data
	if(test_mode==false)
	{
		make_random_corner();
		if(cube_order_%2)
		{
			make_random_middle_edge();
		}
		make_random_face();
		make_random_edgeN();
		PrintState();
		assert(solvable());
	}
	else
	{
		//test
		for(int i=0;i<face24_num_;i++)
		{
			for(int j=0;j<24;j++)
			{
				faces_[i][j]=j/4;
			}
		}
		for(int i=0;i<edge24_num_;i++)
		{
			for(int j=0;j<24;j++)
			{
				edges_[i][j]=j;
			}
		}
		for(int i=0;i<8;i++)
		{
			corner_permutation_[i]=i;
			corner_twist_[i]=0;
		}
		if(cube_order_%2)
		{
			for(int i=0;i<12;i++)
			{
				edge_permutation_[i]=i;
				edge_flip_[i]=0;
			}
			for(int i=0;i<6;i++)
			{
				center_[i]=i;
			}
		}
		//rotate(rand_seed%18);//rotate() test
		/*//rotate() test
		Move move;
		move.direction=5;
		move.index=rand_seed/3;
		if(rand_seed%3==0)
		{
			move.rotation=1;
		}
		else if(rand_seed%3==1)
		{
			move.rotation=2;
		}
		else
		{
			move.rotation=-1;
		}
		rotate(move);
		*/
		/* //formula test
		Move move;
		int x=1;
		int y=1;
		for(int i=0;i<8;i++)
		{
			move.direction=complex_motion_face_moves[rand_seed][i][0]-10;
			move.index=complex_motion_face_moves[rand_seed][i][1];
			move.rotation=complex_motion_face_moves[rand_seed][i][2];
			if(rand_seed<24)
			{
				if(i==0 || i==4)
				{
					move.index=x;
				}
				else if(i==2 || i==6)
				{
					move.index=y;
				}
			}
			else
			{
				if(i==0 || i==4)
				{
					move.index=y;
				}
				else if(i==2 || i==6)
				{
					move.index=x;
				}
			}
			rotate(move);
		}

		PrintState();
		for(int i=0;i<24;i++)
		{
			assert(faces_[0][i]==table_motion_face_[rand_seed+18][i]);
		}
		for(int i=0;i<24;i++)
		{
			assert(edges_[0][i]==i);
		}


		complex_motion_face_append;
		table_motion_face_;
		*/
		/*
		int x=rand_seed+1;
		Move move;
		move.direction=4;
		move.index=x;
		move.rotation=-1;
		rotate(move);
		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=4;
		move.index=x;
		move.rotation=-1;
		rotate(move);
		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=2;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=4;
		move.index=x;
		move.rotation=-1;
		rotate(move);
		move.direction=2;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=1;
		move.index=x;
		move.rotation=1;
		rotate(move);
		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=1;
		move.index=x;
		move.rotation=-1;
		rotate(move);
		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);
		move.direction=4;
		move.index=x;
		move.rotation=2;
		rotate(move);
		*/
		/*//edge formula test
		int x=rand_seed;
		int y=1;
		Move move;
		for(int i=0;i<12;i++)
		{
			move.direction = complex_motion_edge_moves[x][i][0]-10;
			move.index = complex_motion_edge_moves[x][i][1];
			move.rotation = complex_motion_edge_moves[x][i][2];
			if(i==0 || i==2 || i==5 || i==7 || i==9 || i==11)
			{
				move.index=y;
			}
			rotate(move);
		}
		
		PrintState();
		for(int i=0;i<24;i++)
		{
			assert(edges_[0][i]==table_motion_edge_[rand_seed+18][i]);
		}
		for(int i=0;i<face24_num_;i++)
		{
			for(int j=0;j<24;j++)
			{
				assert(faces_[i][j]==j/4);
			}
		}
		table_motion_edge_;
		complex_motion_edge_append;
		*/
/*
		int x=rand_seed+1;
		Move move;
		move.direction=1;
		move.index=x;
		move.rotation=2;
		rotate(move);

		move.direction=5;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=4;
		move.index=x;
		move.rotation=1;
		rotate(move);

		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=1;
		move.index=x;
		move.rotation=-1;
		rotate(move);

		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=1;
		move.index=x;
		move.rotation=1;
		rotate(move);

		move.direction=0;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=2;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=1;
		move.index=x;
		move.rotation=1;
		rotate(move);

		move.direction=2;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=4;
		move.index=x;
		move.rotation=-1;
		rotate(move);

		move.direction=5;
		move.index=0;
		move.rotation=2;
		rotate(move);

		move.direction=1;
		move.index=x;
		move.rotation=2;
		rotate(move);
		*/
		Move move;
		int x=1;
		int i=rand_seed;
		for(int j=0;j<15;j++)
		{
			move.direction	=complex_motion_edge_moves_special[i][j][0]-10;
			move.index		=complex_motion_edge_moves_special[i][j][1];
			if(move.index==-1)
			{
				move.index=x;
			}
			move.rotation	=complex_motion_edge_moves_special[i][j][2];
			rotate(move);
		}
		PrintState();
		for(int i=0;i<face24_num_;i++)
		{
			for(int j=0;j<24;j++)
			{
				assert(faces_[i][j]==j/4);
			}
		}
		rand_seed++;
		if(rand_seed>=24)
		{
			rand_seed=0;
		}
	}


}
void SolveCubeN::MakeRandomCube(Cube *cube)
{


	assert(cube->n()>=4);
	MakeRandomCubeCore(cube->n());


	//step 2, use random data
	fill_corner_pointer(cube);
	if(test_mode==false || rand_seed==1)
	{
		get_color_data(cube);
	}
	corner2cube();
	if(cube_order_%2)
	{
		fill_midedge_pointer(cube);
		midedge2cube();
		if(test_mode==true)
		{
			fill_center_pointer(cube);
			center2cube();
		}
	}
	//face
	int face_row=(cube_order_/2-1);
	//int face_col=(cube_order_-1)/2;
	//assert(face_row*face_col==face24_num_);
	//for(int i=1;i<=face_row;i++)
	//{
	//	for(int j=1;j<=face_col;j++)
	//	{
	//		int index[2][4];
	//		memcpy(index[0],get_4index(i,j,cube_order_),4*sizeof(int));// top right front
	//		memcpy(index[1],get_4index_inverse(j,i,cube_order_),4*sizeof(int));//bottom left back

	//		int face_index=(i-1)*face_col+(j-1);
	//		for(int k=0;k<24;k++)
	//		{
	//			int color=faces_[face_index][k];
	//			color=table_color_[color];
	//			cube->face(k/4)[index[k/12][k%4]]=color;
	//		}
	//	}
	//}
	int face_index=0;
	for(int i=1;i<=face_row;i++)
	{
		for(int j=i;j<cube_order_-1-i;j++)
		{

			int index[2][4];
			memcpy(index[0],get_4index(i,j,cube_order_),4*sizeof(int));// top right front
			memcpy(index[1],get_4index_inverse(j,i,cube_order_),4*sizeof(int));//bottom left back
			for(int k=0;k<24;k++)
			{
				int color=faces_[face_index][k];
				color=table_color_[color];
				cube->face(k/4)[index[k/12][k%4]]=color;
			}
			face_index++;
		}
	}
	assert(face_index==face24_num_);
	//edge
	for(int i=1;i<=edge24_num_;i++)
	{
		int index[2][4];
		memcpy(index[0],get_4index(0,i,cube_order_),4*sizeof(int));// top right front
		memcpy(index[1],get_4index_inverse(i,0,cube_order_),4*sizeof(int));//bottom left back

		for(int face_i=0;face_i<6;face_i++)
		{
			for(int j=0;j<4;j++)
			{
				int index1=index[face_i/3][j];
				int index2;
				if(face_i/3==edge_second_face[face_i][j]/3)
				{
					index2=index[1-face_i/3][j];
				}
				else
				{
					index2=index[face_i/3][j/2*2+1-j%2];
				}
				int id=edges_[i-1][face_i*4+j];
				int color=table_edge_id2color_[id];
				int color1=color/6;
				int color2=color%6;
				color1=table_color_[color1];
				color2=table_color_[color2];
				cube->face(face_i)[index1]=color1;
				cube->face(edge_second_face[face_i][j])[index2]=color2;
				//int color1=cube->face(face_i)[index1];
				//int color2=cube->face(edge_second_face[face_i][j])[index2];
				//color1=table_color_inverse_[color1];
				//color2=table_color_inverse_[color2];
				//edges_[i-1][face_i*4+j]=table_edge_color2id_[color1*6+color2];
			}
		}
	}
	

}
void SolveCubeN::make_random_face()
{
	for(int i=0;i<face24_num_;i++)
	{
		for(int j=0;j<24;j++)
		{
			faces_[i][j]=j/4;
		}
		std::random_shuffle(faces_[i],faces_[i]+24);
	}
}
void SolveCubeN::make_random_edgeN()
{
	for(int i=0;i<edge24_num_;i++)
	{
		for(int j=0;j<24;j++)
		{
			edges_[i][j]=j;
		}
		std::random_shuffle(edges_[i],edges_[i]+24);
	}
}



void SolveCubeN::GetData(Cube *cube)
{
	assert(cube->n()>=4);
	cube_order_=cube->n();

	face24_num_= (cube_order_-2)*(cube_order_-2)/4;
	if(faces_!=NULL)
	{
		free(faces_);
	}
	faces_=(Array24)malloc(face24_num_*24*sizeof(char));
	
	edge24_num_=(cube_order_-2)/2;
	if(edges_!=NULL)
	{
		free(edges_);
	}
	edges_=(Array24)malloc(edge24_num_*24*sizeof(char));

	fill_corner_pointer(cube);	
	if(cube_order_%2)
	{
		fill_midedge_pointer(cube);
	}
	get_color_data(cube);
	//corner_permutation
	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=get_corner_id_from_cube(*corner_pointer_[i][0],*corner_pointer_[i][1],*corner_pointer_[i][2]);
	}
	//corner_twist
	for(int i=0;i<8;i++)
	{
		corner_twist_[i]=get_corner_twist_from_cube(*corner_pointer_[i][0],*corner_pointer_[i][1],*corner_pointer_[i][2]);
	}

	//middle edge,only for odd cube
	if(cube_order_%2)
	{
		for(int i=0;i<12;i++)
		{
			edge_permutation_[i]=get_edge_id_from_cube(*midedge_pointer_[i][0], *midedge_pointer_[i][1]);
		}
		for(int i=0;i<12;i++)
		{
			edge_flip_[i]=get_edge_flip_from_cube(*midedge_pointer_[i][0], *midedge_pointer_[i][1]);
		}
	}
	//face
	int face_row=(cube_order_/2-1);
	//int face_col=(cube_order_-1)/2;
	//assert(face_row*face_col==face24_num_);
	//for(int i=1;i<=face_row;i++)
	//{
	//	for(int j=1;j<=face_col;j++)
	//	{
	//		int index[2][4];
	//		memcpy(index[0],get_4index(i,j,cube_order_),4*sizeof(int));// top right front
	//		memcpy(index[1],get_4index_inverse(j,i,cube_order_),4*sizeof(int));//bottom left back
	//		
	//		int face_index=(i-1)*face_col+(j-1);
	//		for(int k=0;k<24;k++)
	//		{
	//			int color=cube->face(k/4)[index[k/12][k%4]];
	//			color=table_color_inverse_[color];
	//			faces_[face_index][k]=color;
	//		}
	//	}
	//}
	int face_index=0;
	for(int i=1;i<=face_row;i++)
	{
		for(int j=i;j<cube_order_-1-i;j++)
		{
			
			int index[2][4];
			memcpy(index[0],get_4index(i,j,cube_order_),4*sizeof(int));// top right front
			memcpy(index[1],get_4index_inverse(j,i,cube_order_),4*sizeof(int));//bottom left back
			for(int k=0;k<24;k++)
			{
				int color=cube->face(k/4)[index[k/12][k%4]];
				color=table_color_inverse_[color];
				faces_[face_index][k]=color;
			}
			face_index++;
		}
	}
	assert(face_index==face24_num_);
	//edge
	for(int i=1;i<=edge24_num_;i++)
	{
		int index[2][4];
		memcpy(index[0],get_4index(0,i,cube_order_),4*sizeof(int));// top right front
		memcpy(index[1],get_4index_inverse(i,0,cube_order_),4*sizeof(int));//bottom left back

		for(int face_i=0;face_i<6;face_i++)
		{
			for(int j=0;j<4;j++)
			{
				int index1=index[face_i/3][j];
				int index2;
				if(face_i/3==edge_second_face[face_i][j]/3)
				{
					index2=index[1-face_i/3][j];
				}
				else
				{
					index2=index[face_i/3][j/2*2+1-j%2];
				}
				int color1=cube->face(face_i)[index1];
				int color2=cube->face(edge_second_face[face_i][j])[index2];
				color1=table_color_inverse_[color1];
				color2=table_color_inverse_[color2];
				edges_[i-1][face_i*4+j]=table_edge_color2id_[color1*6+color2];
			}
		}
	}
	
	PrintState();
	assert(solvable());
}
void SolveCubeN::PrintState()
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
	if(cube_order_%2)
	{
		std::cout<<"edge = {";
		for(int i=0;i<11;i++)
		{
			std::cout<<(int)edge_permutation_[i]+1<<",";	
		}
		std::cout<<(int)edge_permutation_[11]+1<<"}"<<std::endl;
		std::cout<<"edge_flip_ = {";
		for(int i=0;i<11;i++)
		{
			std::cout<<(int)edge_flip_[i]<<",";	
		}
		std::cout<<(int)edge_flip_[11]<<"}"<<std::endl;
	}
	
	std::cout<<"face = "<<std::endl;
	for(int i=0;i<face24_num_;i++)
	{
		std::cout<<"{";
		for(int j=0;j<23;j++)
		{
			std::cout<<(int)faces_[i][j]<<",";
		}
		std::cout<<(int)faces_[i][23]<<"}"<<std::endl;
	}
	std::cout<<"edge = "<<std::endl;
	for(int i=0;i<edge24_num_;i++)
	{
		std::cout<<"{";
		for(int j=0;j<23;j++)
		{
			std::cout<<(int)edges_[i][j]<<",";
		}
		std::cout<<(int)edges_[i][23]<<"}"<<std::endl;
	}
}

void SolveCubeN::get_color_data(Cube *cube)
{
	assert(cube);
	if(cube_order_%2)
	{
		get_color_data_odd(cube);
	}
	else
	{
		get_color_data_even(cube);
	}
}
void SolveCubeN::get_color_data_odd(Cube *cube)// same to cube3
{
	assert(cube);
	int n=cube->n();
	assert(n%2);

	int center_index=(n*n)/2;
	table_color_[0]=cube->top()[center_index];
	table_color_[1]=cube->right()[center_index];
	table_color_[2]=cube->front()[center_index];
	table_color_[3]=cube->bottom()[center_index];
	table_color_[4]=cube->left()[center_index];
	table_color_[5]=cube->back()[center_index];

	for(int i=0;i<6;i++)
	{
		table_color_inverse_[table_color_[i]]=i;
	}
}

void SolveCubeN::get_color_data_even(Cube*cube) //totally same to cube2
{
	assert(cube);
	assert(cube->n()%2==0);

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


/*
static char basic_motion_face[6][24]=
{
	{3,0,1,2 ,4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 7,4,5,6, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 11,8,9,10, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 15,12,13,14, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 19,16,17,18  20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 23,20,21,22},
};

static char basic_motion_edge[6][24]=
{
	{3,0,1,2 ,4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 7,4,5,6, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 11,8,9,10, 12,13,14,15, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 15,12,13,14, 16,17,18,19, 20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 19,16,17,18  20,21,22,23},
	{0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 23,20,21,22},
};
*/

static char basic_motion_edge_append[6][4]=
{
	{22,6,9,17},
	{14,10,1,21},
	{18,2,5,13},
	{19,11,4,20},
	{23,3,8,12},
	{15,7,0,16},
};

static char middle_rotation_center[6][6]=
{
	{0,5,1,3,2,4},
	{2,1,3,5,4,0},
	{4,0,2,1,3,5},
	{0,2,4,3,5,1},
	{5,1,0,2,4,3},
	{1,3,2,4,0,5},
};
static char middle_rotation_middle_edge_permutation[6][12]=
{
	{0,1,2,3,4,5,6,7,11,8,9,10},
	{0,5,2,1,4,7,6,3,8,9,10,11},
	{2,1,6,3,0,5,4,7,8,9,10,11},
	{0,1,2,3,4,5,6,7,9,10,11,8},
	{0,3,2,7,4,1,6,5,8,9,10,11},
	{4,1,0,3,6,5,2,7,8,9,10,11},
};
static char middle_rotation_middle_edge_flip[6][12]=
{
	{0,0,0,0, 0,0,0,0 ,1,1,1,1},
	{0,1,0,1, 0,1,0,1 ,0,0,0,0},
	{1,0,1,0, 1,0,1,0 ,0,0,0,0},
	{0,0,0,0, 0,0,0,0 ,1,1,1,1},
	{0,1,0,1, 0,1,0,1 ,0,0,0,0},
	{1,0,1,0, 1,0,1,0 ,0,0,0,0},
};


char SolveCubeN::table_motion_face_append_[18][3][24];
char SolveCubeN::table_motion_face_append_inv_[18][3][24];
char SolveCubeN::table_motion_edge_append_[18][24];//arbitrary move, except face move
char SolveCubeN::table_motion_edge_append_inv_[18][24];//arbitrary move,except face move

char SolveCubeN::table_motion_middle_rotation_center_[18][6];
char SolveCubeN::table_motion_middle_rotation_middle_edge_permutation_[18][12];
char SolveCubeN::table_motion_middle_rotation_middle_edge_flip_[18][12];
char SolveCubeN::table_motion_face_[66][24];
char SolveCubeN::table_motion_edge_[66][24];
char SolveCubeN::table_motion_face_inv_[66][24];
char SolveCubeN::table_motion_edge_inv_[66][24];

int SolveCubeN::table_transport_face_[23*22*21][66];//24*23*22*21/24
int SolveCubeN::table_transport_3face_[495][66];
int SolveCubeN::table_transport_edge_[24][66];

char SolveCubeN::table_depth_face_[6][23*22*21];
char SolveCubeN::table_premove_face_[6][23*22*21];
int  SolveCubeN::table_prestate_face_[6][23*22*21];
char SolveCubeN::table_depth_edge_[24][24];
char SolveCubeN::table_premove_edge_[24][24];
int  SolveCubeN::table_prestate_edge_[24][24];

char SolveCubeN::table_depth_3face_[245025];//12!/24^3
char SolveCubeN::table_premove_3face[245025];
int  SolveCubeN::table_prestate_3face[245025];

inline static void get_neibor(char *x)
{
	assert(x[0]>=0 && x[0]<24);
	x[1]= x[0]/4*4 + (x[0]+1)%4;
	x[2]= x[0]/4*4 + (x[0]+3)%4;
}
inline static char get_right_neibor(char x)
{
	assert(x>=0 && x<24);
	return (x/4*4 + (x+1)%4);
}
inline static char get_left_neibor(char x)
{
	assert(x>=0 && x<24);
	return (x/4*4 + (x+3)%4);
}
void SolveCubeN::make_table_motion()
{
	char a[24];
	for(int i=0;i<24;i++)
	{
		a[i]=i;
	}
	//face_move : face
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_face_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3*4;
		for(int j=0;j<4;j++)
		{
			table_motion_face_[i][target_face+(j+offset)%4]=a[target_face+j];
		}
	}
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_face_inv_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3*4;
		for(int j=0;j<4;j++)
		{
			table_motion_face_inv_[i][target_face+(j)]=a[target_face+(j+offset)%4];
		}
	}
	//face_move : edge
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_edge_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;
		for(int j=0;j<4;j++)
		{
			table_motion_edge_[i][target_face*4+(j+offset)%4]=a[target_face*4+j];
		}
		for(int j=0;j<4;j++)
		{
			table_motion_edge_[i][basic_motion_edge_append[target_face][(j+offset)%4]]
			= a[basic_motion_edge_append[target_face][j]];
		}
	}
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_edge_inv_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;
		for(int j=0;j<4;j++)
		{
			table_motion_edge_inv_[i][target_face*4+(j)]=a[target_face*4+(j+offset)%4];
		}
		for(int j=0;j<4;j++)
		{
			table_motion_edge_inv_[i][basic_motion_edge_append[target_face][j]]
			= a[basic_motion_edge_append[target_face][(j+offset)%4]];
		}
	}

	//arbitrary move, except face move : face append
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_face_append_[i][0],a,sizeof(a));
		memcpy(table_motion_face_append_[i][1],a,sizeof(a));
		memcpy(table_motion_face_append_[i][2],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;

		/*
		for(int j=0;j<4;j++)
		{
			table_motion_face_append_[i][basic_motion_edge_append[target_face][(j+offset)%4]]
			= basic_motion_edge_append[target_face][j];
		}
		*/
		char x[3];
		char y[3];
		for(int j=0;j<4;j++)
		{
			x[0]=basic_motion_edge_append[target_face][(j+offset)%4];
			y[0]=basic_motion_edge_append[target_face][j];
			get_neibor(x);
			get_neibor(y);
			for(int k=0;k<3;k++)
			{
				table_motion_face_append_[i][k][x[k]]=y[k];
			}
		}
	}
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_face_append_inv_[i][0],a,sizeof(a));
		memcpy(table_motion_face_append_inv_[i][1],a,sizeof(a));
		memcpy(table_motion_face_append_inv_[i][2],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;
		/*
		for(int j=0;j<4;j++)
		{
			table_motion_face_append_inv_[i][basic_motion_edge_append[target_face][j]]
			= a[basic_motion_edge_append[target_face][(j+offset)%4]];
		}*/
		char x[3];
		char y[3];
		for(int j=0;j<4;j++)
		{
			x[0]=basic_motion_edge_append[target_face][(j+offset)%4];
			y[0]=basic_motion_edge_append[target_face][j];
			get_neibor(x);
			get_neibor(y);
			for(int k=0;k<3;k++)
			{
				table_motion_face_append_inv_[i][k][y[k]]=x[k];
			}
		}
	}
	//arbitrary move, except face move : edge append
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_edge_append_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;
		int x,y;
		for(int j=0;j<4;j++)
		{
			x=basic_motion_edge_append[target_face][(j+offset)%4];
			y=basic_motion_edge_append[target_face][j];
			x= get_right_neibor(x);
			y= get_right_neibor(y);
			table_motion_edge_append_[i][x]=y;
		}
	}
	for(int i=0;i<18;i++)
	{
		memcpy(table_motion_edge_append_inv_[i],a,sizeof(a));
		int offset=i%3+1;
		int target_face=i/3;
		int x,y;
		for(int j=0;j<4;j++)
		{
			x=basic_motion_edge_append[target_face][(j+offset)%4];
			y=basic_motion_edge_append[target_face][j];
			x= get_right_neibor(x);
			y= get_right_neibor(y);
			table_motion_edge_append_inv_[i][y]=x;
		}
	}

	//face formula
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			int y=basic_motion_edge_append[i][(j+3)%4];
			complex_motion_face_append[x][0] = x;
			complex_motion_face_append[x][1] = y/4*4+(y+3)%4;
			complex_motion_face_append[x][2] = basic_motion_edge_append[(i+3)%6][(4-j)%4];
		}
	}
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j+24;
			int y= basic_motion_edge_append[(i+3)%6][(3-j)%4];
			complex_motion_face_append[x][0] = x-24;
			complex_motion_face_append[x][1] = y;
			complex_motion_face_append[x][2] = y/4*4+(y+1)%4;
		}
	}
	for(int i=0;i<48;i++)
	{
		memcpy(table_motion_face_[i+18],a,sizeof(a));
		for(int j=0;j<3;j++)
		{
			table_motion_face_[i+18][complex_motion_face_append[i][j]]=complex_motion_face_append[i][(j+1)%3];
		}
	}
	for(int i=0;i<48;i++)
	{
		memcpy(table_motion_face_inv_[i+18],a,sizeof(a));
		for(int j=0;j<3;j++)
		{
			table_motion_face_inv_[i+18][complex_motion_face_append[i][(j+1)%3]]=complex_motion_face_append[i][j];
		}
	}
	//face formula : face motion translate
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			//1
			complex_motion_face_moves[x][0][0]=basic_motion_edge_append[i][j]/4+10;
			complex_motion_face_moves[x][0][1]=-1;// uncertain, need to be filled later,i
			complex_motion_face_moves[x][0][2]=-1;
			//2
			complex_motion_face_moves[x][1][0]=i+10;
			complex_motion_face_moves[x][1][1]=0;
			complex_motion_face_moves[x][1][2]=-1;
			//3
			complex_motion_face_moves[x][2][0]=(basic_motion_edge_append[i][j]/4+3)%6+10;
			complex_motion_face_moves[x][2][1]=-2;// uncertain, need to be filled later,j
			complex_motion_face_moves[x][2][2]=1;
			//4
			complex_motion_face_moves[x][3][0]=10+i;
			complex_motion_face_moves[x][3][1]=0;
			complex_motion_face_moves[x][3][2]=1;
			//5
			complex_motion_face_moves[x][4][0]=basic_motion_edge_append[i][j]/4+10;
			complex_motion_face_moves[x][4][1]=-1;// uncertain, need to be filled later,i
			complex_motion_face_moves[x][4][2]=1;
			//6
			complex_motion_face_moves[x][5][0]=10+i;
			complex_motion_face_moves[x][5][1]=0;
			complex_motion_face_moves[x][5][2]=-1;
			//7
			complex_motion_face_moves[x][6][0]=(basic_motion_edge_append[i][j]/4+3)%6+10;
			complex_motion_face_moves[x][6][1]=-2;// uncertain, need to be filled later,j
			complex_motion_face_moves[x][6][2]=-1;
			//8
			complex_motion_face_moves[x][7][0]=10+i;
			complex_motion_face_moves[x][7][1]=0;
			complex_motion_face_moves[x][7][2]=1;
		}
	}
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j+24;
			//1
			complex_motion_face_moves[x][0][0]=basic_motion_edge_append[i][(j+3)%4]/4+10;
			complex_motion_face_moves[x][0][1]=-2;// uncertain, need to be filled later,j
			complex_motion_face_moves[x][0][2]=1;
			//2
			complex_motion_face_moves[x][1][0]=i+10;
			complex_motion_face_moves[x][1][1]=0;
			complex_motion_face_moves[x][1][2]=1;
			//3
			complex_motion_face_moves[x][2][0]=basic_motion_edge_append[i][(j+1)%4]/4+10;
			complex_motion_face_moves[x][2][1]=-1;// uncertain, need to be filled later,i
			complex_motion_face_moves[x][2][2]=-1;
			assert(complex_motion_face_moves[x][2][0]-10==(complex_motion_face_moves[x][0][0]-10+3)%6);
			//4
			complex_motion_face_moves[x][3][0]=10+i;
			complex_motion_face_moves[x][3][1]=0;
			complex_motion_face_moves[x][3][2]=-1;
			//5
			complex_motion_face_moves[x][4][0]=basic_motion_edge_append[i][(j+3)%4]/4+10;
			complex_motion_face_moves[x][4][1]=-2;// uncertain, need to be filled later,j
			complex_motion_face_moves[x][4][2]=-1;
			//6
			complex_motion_face_moves[x][5][0]=10+i;
			complex_motion_face_moves[x][5][1]=0;
			complex_motion_face_moves[x][5][2]=1;
			//7
			complex_motion_face_moves[x][6][0]=basic_motion_edge_append[i][(j+1)%4]/4+10;
			complex_motion_face_moves[x][6][1]=-1;// uncertain, need to be filled later,i
			complex_motion_face_moves[x][6][2]=1;
			//8
			complex_motion_face_moves[x][7][0]=10+i;
			complex_motion_face_moves[x][7][1]=0;
			complex_motion_face_moves[x][7][2]=-1;
		}
	}
	//edge formula
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			complex_motion_edge_append[x][0] = x;
			complex_motion_edge_append[x][1] = basic_motion_edge_append[i][(j+2)%4];
		}
	}
	for(int i=0;i<24;i++)
	{
		memcpy(table_motion_edge_[i+18],a,sizeof(a));
		for(int j=0;j<2;j++)
		{
			table_motion_edge_[i+18][complex_motion_edge_append[i][j]]=complex_motion_edge_append[i][(j+1)%2];
		}
	}
	for(int i=0;i<24;i++)
	{
		memcpy(table_motion_edge_inv_[i+18],a,sizeof(a));
		for(int j=0;j<2;j++)
		{
			table_motion_edge_inv_[i+18][complex_motion_edge_append[i][j]]=complex_motion_edge_append[i][(j+1)%2];
		}
	}
	//edge formula ,special
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			complex_motion_edge_append_special[x][0] = x;
			complex_motion_edge_append_special[x][1] = basic_motion_edge_append[i][j];
		}
	}
	//edge formula : motion translate
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			int direction1=basic_motion_edge_append[i][(j+1)%4]/4+10;
			int direction2=basic_motion_edge_append[i][(j+3)%4]/4+10;
			int direction3=basic_motion_edge_append[i][(j+2)%4]/4+10;
			assert((direction1+3-10)%6==direction2-10);
			//1
			complex_motion_edge_moves[x][0][0]=direction1;
			complex_motion_edge_moves[x][0][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][0][2]=1;
			//2
			complex_motion_edge_moves[x][1][0]=i+10;
			complex_motion_edge_moves[x][1][1]=0;
			complex_motion_edge_moves[x][1][2]=2;
			//3
			complex_motion_edge_moves[x][2][0]=direction1;
			complex_motion_edge_moves[x][2][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][2][2]=1;
			//4
			complex_motion_edge_moves[x][3][0]=10+i;
			complex_motion_edge_moves[x][3][1]=0;
			complex_motion_edge_moves[x][3][2]=2;
			//5
			complex_motion_edge_moves[x][4][0]=direction3;
			complex_motion_edge_moves[x][4][1]=0;
			complex_motion_edge_moves[x][4][2]=2;
			//6
			complex_motion_edge_moves[x][5][0]=direction1;
			complex_motion_edge_moves[x][5][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][5][2]=1;
			//7
			complex_motion_edge_moves[x][6][0]=direction3;
			complex_motion_edge_moves[x][6][1]=0;
			complex_motion_edge_moves[x][6][2]=2;
			//8
			complex_motion_edge_moves[x][7][0]=direction2;
			complex_motion_edge_moves[x][7][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][7][2]=-1;
			//9
			complex_motion_edge_moves[x][8][0]=i+10;
			complex_motion_edge_moves[x][8][1]=0;
			complex_motion_edge_moves[x][8][2]=2;
			//10
			complex_motion_edge_moves[x][9][0]=direction2;
			complex_motion_edge_moves[x][9][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][9][2]=1;
			//11
			complex_motion_edge_moves[x][10][0]=i+10;
			complex_motion_edge_moves[x][10][1]=0;
			complex_motion_edge_moves[x][10][2]=2;
			//12
			complex_motion_edge_moves[x][11][0]=direction1;
			complex_motion_edge_moves[x][11][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves[x][11][2]=2;
		}
	}
	//edge formula, special, motion translate
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<4;j++)
		{
			int x=i*4+j;
			int direction0=i+10;
			int direction1=basic_motion_edge_append[i][(j+3)%4]/4+10;
			int direction2=basic_motion_edge_append[i][j]/4+10;
			int direction4=basic_motion_edge_append[i][(j+1)%4]/4+10;
			int direction5=basic_motion_edge_append[i][(j+2)%4]/4+10;
			assert((direction1+3-10)%6==direction4-10);
			assert((direction2+3-10)%6==direction5-10);
			//1
			complex_motion_edge_moves_special[x][0][0]=direction1;
			complex_motion_edge_moves_special[x][0][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][0][2]=2;
			//2
			complex_motion_edge_moves_special[x][1][0]=direction5;
			complex_motion_edge_moves_special[x][1][1]=0;
			complex_motion_edge_moves_special[x][1][2]=2;
			//3
			complex_motion_edge_moves_special[x][2][0]=direction0;
			complex_motion_edge_moves_special[x][2][1]=0;
			complex_motion_edge_moves_special[x][2][2]=2;
			//4
			complex_motion_edge_moves_special[x][3][0]=direction4;
			complex_motion_edge_moves_special[x][3][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][3][2]=1;
			//5
			complex_motion_edge_moves_special[x][4][0]=direction0;
			complex_motion_edge_moves_special[x][4][1]=0;
			complex_motion_edge_moves_special[x][4][2]=2;
			//6
			complex_motion_edge_moves_special[x][5][0]=direction1;
			complex_motion_edge_moves_special[x][5][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][5][2]=-1;
			//7
			complex_motion_edge_moves_special[x][6][0]=direction0;
			complex_motion_edge_moves_special[x][6][1]=0;
			complex_motion_edge_moves_special[x][6][2]=2;
			//8
			complex_motion_edge_moves_special[x][7][0]=direction1;
			complex_motion_edge_moves_special[x][7][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][7][2]=1;
			//9
			complex_motion_edge_moves_special[x][8][0]=direction0;
			complex_motion_edge_moves_special[x][8][1]=0;
			complex_motion_edge_moves_special[x][8][2]=2;
			//10
			complex_motion_edge_moves_special[x][9][0]=direction2;
			complex_motion_edge_moves_special[x][9][1]=0;
			complex_motion_edge_moves_special[x][9][2]=2;
			//11
			complex_motion_edge_moves_special[x][10][0]=direction1;
			complex_motion_edge_moves_special[x][10][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][10][2]=1;
			//12
			complex_motion_edge_moves_special[x][11][0]=direction2;
			complex_motion_edge_moves_special[x][11][1]=0;
			complex_motion_edge_moves_special[x][11][2]=2;
			//13
			complex_motion_edge_moves_special[x][12][0]=direction4;
			complex_motion_edge_moves_special[x][12][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][12][2]=-1;
			//14
			complex_motion_edge_moves_special[x][13][0]=direction5;
			complex_motion_edge_moves_special[x][13][1]=0;
			complex_motion_edge_moves_special[x][13][2]=2;
			//15
			complex_motion_edge_moves_special[x][14][0]=direction1;
			complex_motion_edge_moves_special[x][14][1]=-1;// uncertain, need to be filled later
			complex_motion_edge_moves_special[x][14][2]=2;
		}
	}
	//arbitrary move : center, odd cube only
	for(int i=0;i<6;i++)
	{
		char *temp;
		memcpy(table_motion_middle_rotation_center_[3*i],
			middle_rotation_center[i],6);
		temp=multiply_permutation(middle_rotation_center[i],
			middle_rotation_center[i],6);
		memcpy(table_motion_middle_rotation_center_[3*i+1],temp,6);
		temp=multiply_permutation(table_motion_middle_rotation_center_[3*i+1],
			middle_rotation_center[i],6);
		memcpy(table_motion_middle_rotation_center_[3*i+2],temp,6);
	}
	//arbitrary move : middle_edge, odd cube only
	for(int i=0;i<6;i++)
	{
		char *temp;
		memcpy(table_motion_middle_rotation_middle_edge_permutation_[i*3],
			middle_rotation_middle_edge_permutation[i],12);
		temp=multiply_permutation(middle_rotation_middle_edge_permutation[i],
			middle_rotation_middle_edge_permutation[i],12);
		memcpy(table_motion_middle_rotation_middle_edge_permutation_[i*3+1],temp,12);
		temp=multiply_permutation(table_motion_middle_rotation_middle_edge_permutation_[i*3+1],
			middle_rotation_middle_edge_permutation[i],12);
		memcpy(table_motion_middle_rotation_middle_edge_permutation_[i*3+2],temp,12);	
	}
	for(int i=0;i<6;i++)
	{
		char *temp;
		memcpy(table_motion_middle_rotation_middle_edge_flip_[i*3],
			middle_rotation_middle_edge_flip[i],12);
		temp=multiply_twist(middle_rotation_middle_edge_flip[i],
			middle_rotation_middle_edge_flip[i],
			middle_rotation_middle_edge_permutation[i],12,2);
		memcpy(table_motion_middle_rotation_middle_edge_flip_[i*3+1],temp,12);
		temp=multiply_twist(table_motion_middle_rotation_middle_edge_flip_[i*3+1],
			middle_rotation_middle_edge_flip[i],
			middle_rotation_middle_edge_permutation[i],12,2);
		memcpy(table_motion_middle_rotation_middle_edge_flip_[i*3+2],temp,12);	
	}
	//test
	/*
	char b[24];
	char bb[24];
	//char c[]={3,18,14,20,5,18,12,20};
	char c[]={3};
	char temp[24];

	memcpy(b,a,sizeof(a));
	for(int i=0;i<24;i++)
	{
		bb[i]=i/4;
	}
	for(int i=0;i<sizeof(c);i++)
	{
		char * move;
		if(c[i]>=18)
		{
			move=table_motion_face_[c[i]%18];
		}
		else
		{
			move=table_motion_face_append_[c[i]];
		}
		for(int j=0;j<24;j++)
		{
			temp[j]=b[move[j]];
		}
		memcpy(b,temp,24);
		for(int j=0;j<24;j++)
		{
			temp[j]=bb[move[j]];
		}
		memcpy(bb,temp,24);
	}
	for(int i=0;i<24;i++)
	{
		std::cout<<(int)b[i]<<",";
	}
	std::cout<<std::endl;
	for(int i=0;i<24;i++)
	{
		std::cout<<(int)bb[i]<<",";
	}
	std::cout<<std::endl;
*/
}
void SolveCubeN::make_table_transport()
{
	char a[4];
	char b[4];
	int r;

	//face
	int n=10626;//24*23*22*21/24;
	for(int i=0;i<n;i++)
	{
		
		Permutation::UnRankSubset(i,4,24,a);
		for(int j=0;j<66;j++)
		{
			for(int k=0;k<4;k++)
			{
				b[k]=table_motion_face_inv_[j][a[k]];
			}
			std::sort(b,b+4);
			assert(b[0]<b[1]&& b[1]<b[2] && b[2]<b[3]);
			r= Permutation::RankSubset(b,4,24);
			table_transport_face_[i][j]=r;
		}
	}
	//following code block is useless
	n=495;
	for(int i=0;i<495;i++)
	{
		Permutation::UnRankSubset(i,4,12,a);
		for(int j=18;j<66;j++)
		{
			if (complex_motion_face_append[j-18][0]/4>2)
			{
				continue;
			}
			if (complex_motion_face_append[j-18][2]/4>2)
			{
				continue;
			}
			for(int k=0;k<4;k++)
			{
				b[k]=table_motion_face_inv_[j][a[k]];
			}
			std::sort(b,b+4);
			assert(b[0]<b[1]&& b[1]<b[2] && b[2]<b[3]);
			assert(b[3]<12);
			r= Permutation::RankSubset(b,4,12);
			table_transport_3face_[i][j]=r;
		}
	}
	//edge
	/*
	n=24*23;
	for(int i=0;i<n;i++)
	{
		Permutation::UnRankPermutation(i,2,24,a);
		for(int j=0;j<18;j++)
		{
			for(int k=0;k<2;k++)
			{
				b[k]=table_motion_edge_inv_[j][a[k]];
			}
			r=Permutation::RankPermutation(b,2,24);
			table_transport_edge_[i][j]=r;
		}
	}*/
	for(int i=0;i<24;i++)
	{
		for(int j=0;j<42;j++)
		{
			table_transport_edge_[i][j]=table_motion_edge_inv_[j][i];
		}
	}
}

void make_table_trun_face(int face,int origin_state,char* table_depth,char *table_pre_move,int* table_pre_state, int  table_transport[][66])
{
	std::deque<int>temp;
	int n=10626;//24*23*22*21/24;
	memset(table_depth,-1,n);
	table_depth[origin_state]=0;
	table_pre_move[origin_state]=-1;
	table_pre_state[origin_state]=-1;
	int count=1;
	temp.push_back(origin_state);
	int x,y;
	int deep;
	while(temp.empty()==false)
	{
		x=temp.front();
		temp.pop_front();
		deep=table_depth[x];
		for(int i=0;i<66;i++)
		{
			if(i>=18)
			{
				if(complex_motion_face_append[i-18][0]/4>face)
				{
					continue;
				}
				if(complex_motion_face_append[i-18][2]/4>face)
				{
					continue;
				}
			}
			y=table_transport[x][i];
			if(table_depth[y]==-1)
			{
				temp.push_back(y);
				table_depth[y]=deep+1;
				table_pre_move[y]=i;
				table_pre_state[y]=x;
				count++;
			}
		}
	}
	int count_should_be=Permutation::binomial[24-(5-face)*4][4];
	assert(count==count_should_be);
	std::cout<<"SolveCube face max depth : "<<deep<<std::endl;
}

void make_table_trun_3face(char* table_depth,char *table_pre_move,int* table_pre_state, int  table_transport[][66])
{
	std::deque<int>temp;
	int n=34650;//12!/24^3
	memset(table_depth,-1,n);

	int orgin_state=0;
	char a[4];
	char b[4];
	for(int i=0;i<4;i++)
	{
		a[i]=i;
		b[i]=i+4;
	}
	orgin_state=Permutation::RankSubset(b,4,12);
	table_depth[orgin_state]=0;
	table_pre_move[orgin_state]=-1;
	table_pre_state[orgin_state]=-1;
	int count=1;
	temp.push_back(orgin_state);
	int x,y;
	int state1;
	int state2;
	int deep;
	while(temp.empty()==false)
	{
		x=temp.front();
		temp.pop_front();
		state1=x/495;
		state2=x%495;

		deep=table_depth[x];
		for(int i=18;i<66;i++)
		{
			if(complex_motion_face_append[i-18][0]/4>2)
			{
				continue;
			}
			if(complex_motion_face_append[i-18][2]/4>2)
			{
				continue;
			}
			state1=table_transport[state1][i];
			state2=table_transport[state2][i];

			y=state1*495+state2;
			if(table_depth[y]==-1)
			{
				temp.push_back(y);
				table_depth[y]=deep+1;
				table_pre_move[y]=i;
				table_pre_state[y]=x;
				count++;
			}
		}
	}
	int count_should_be=(Permutation::binomial[12][4])*(Permutation::binomial[8][4]);
	assert(count==count_should_be);
	std::cout<<"SolveCube face max depth : "<<deep<<std::endl;
}

void make_table_trun_edge(int origin_state,char* table_depth,char *table_pre_move,int* table_pre_state, int  table_transport[][66])
{
	std::deque<int>temp;
	int n=24;//24*23*22*21/24;
	memset(table_depth,-1,n);
	table_depth[origin_state]=0;
	table_pre_move[origin_state]=-1;
	table_pre_state[origin_state]=-1;
	int count=1;
	int x,y;
	int deep;
	int forbid1 = origin_state/4;
	int forbid2 = (basic_motion_edge_append[origin_state/4][origin_state%4]) / 4;
	for(int i=18;i<42;i++)
	{
		y=table_transport[origin_state][i];
		if(y!=origin_state)
		{
			assert(table_depth[y]==-1);
			temp.push_back(y);
			table_depth[y]=1;
			table_pre_move[y]=i;
			table_pre_state[y]=origin_state;
			count++;
		}
	}
	while(temp.empty()==false)
	{
		x=temp.front();
		temp.pop_front();
		deep=table_depth[x];
		for(int i=0;i<18;i++)
		{
			if(i/3==forbid1 || i/3==forbid2)
			{
				continue;
			}
			y=table_transport[x][i];
			if(table_depth[y]==-1)
			{
				temp.push_back(y);
				table_depth[y]=deep+1;
				table_pre_move[y]=i;
				table_pre_state[y]=x;
				assert(table_pre_move[x]/3!=table_pre_move[y]/3);
				count++;
			}
		}
	}
	assert(table_depth[basic_motion_edge_append[origin_state/4][origin_state%4]]==-1);
	int count_should_be=24-1;
	assert(count==count_should_be);
	std::cout<<"SolveCube edge max depth : "<<deep<<std::endl;
}

void SolveCubeN::make_table_trun()
{
	//face
	int orgin;
	char a[4];
	for(int i=5;i>=0;i--)
	{
		for(int j=0;j<4;j++)
		{
			a[j]=j+i*4;
		}
		orgin=Permutation::RankSubset(a,4,24);
		make_table_trun_face(i,orgin,table_depth_face_[i],table_premove_face_[i],table_prestate_face_[i],table_transport_face_);
	}
	//make_table_trun_3face(table_depth_3face_,table_premove_3face,table_prestate_3face,table_transport_3face_);
	//edge
	for(int i=0;i<24;i++)
	{
		orgin=i;
		make_table_trun_edge(orgin,table_depth_edge_[i],table_premove_edge_[i],table_prestate_edge_[i],table_transport_edge_);
	}
}
void SolveCubeN::rotate(int move)
{
	assert(move>=0 && move<18);

	char*temp;
	for(int i=0;i<face24_num_;i++)
	{
		temp = multiply_permutation(faces_[i],table_motion_face_[move],24);
		memcpy(faces_[i],temp,24);
	}
	for(int i=0;i<edge24_num_;i++)
	{
		temp = multiply_permutation(edges_[i],table_motion_edge_[move],24);
		memcpy(edges_[i],temp,24);
	}
	temp=multiply_permutation(corner_permutation_,SolveCube3::table_motion_corner[move],8);
	memcpy(corner_permutation_,temp,8);

	temp=multiply_twist(corner_twist_,SolveCube3::table_motion_twist[move],SolveCube3::table_motion_corner[move],8,3);
	memcpy(corner_twist_,temp,8);

	//if(cube_order_%2) //even cube need to process as odd cube
	{
		temp=multiply_permutation(edge_permutation_,SolveCube3::table_motion_edge[move],12);
		memcpy(edge_permutation_,temp,12);
		temp=multiply_twist(edge_flip_,SolveCube3::table_motion_flip[move],SolveCube3::table_motion_edge[move],12,2);
		memcpy(edge_flip_,temp,12);
	}
}
static int get_move_id(const Move &move)
{
	return (move.direction*3+(move.rotation+3)%4);
}
void SolveCubeN::rotate_face(Move move)
{
	assert(move.index>=1 && move.index<=cube_order_-2 && move.index<=(cube_order_-1)/2);
	assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
	assert(move.direction>=0 && move.direction<=5);

	int i=move.index;
	int n=cube_order_;
	int move_id= get_move_id(move);
	for(int j=1;j<=cube_order_-2;j++)
	{
		int x=i;
		int y=j;
		int region=0;
		int index;
		if(j<=i-1)
		{
			x=j;
			y=(n-1-i);
			region=2;
		}
		else if(j>=n-i-1)
		{
			if(j*2<n)
			{
				assert(n%2);
				continue;
			}
			x=n-1-j;
			y=i;
			region=1;
		}
		index=0;
		for(int k=1;k<x;k++)
		{
			index+=n-1-k*2;
		}
		index+=y-x;	
		char *temp=multiply_permutation(faces_[index],table_motion_face_append_[move_id][region],24);
		memcpy(faces_[index],temp,24);
	}
}
void SolveCubeN::rotate_edge(Move move)
{
	assert(move.index>=1 && move.index<=cube_order_-2 && move.index<=(cube_order_-1)/2);
	assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
	assert(move.direction>=0 && move.direction<=5);

	int index=move.index-1;
	int move_id=get_move_id(move);
	char*temp=multiply_permutation(edges_[index], table_motion_edge_append_[move_id],24);
	memcpy(edges_[index],temp,24);
}
void SolveCubeN::rotate(Move move)
{
	assert(move.index>=0 && move.index<=cube_order_-1);
	assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
	assert(move.direction>=0 && move.direction<=5);
	//adjust, index
	if(move.index>(cube_order_-1)/2)
	{
		move.direction=(move.direction+3)%6;
		move.index=cube_order_-1-move.index;
		assert(move.index<=(cube_order_-1)/2);
		if(move.rotation!=2)
		{
			move.rotation=-move.rotation;
		}
	}

	if(move.index==0)
	{
		rotate(move.direction*3+(move.rotation+3)%4);
		return;
	}
	else if(cube_order_%2 && move.index==cube_order_/2)
	{
		char *temp;
		int x=move.direction*3+(move.rotation+3)%4;
		//notice : result of get_color() may change !
		//center
		temp=multiply_permutation(center_,table_motion_middle_rotation_center_[x],6);
		memcpy(center_,temp,6);
		//edge_permutation
		temp=multiply_permutation(edge_permutation_,table_motion_middle_rotation_middle_edge_permutation_[x],12);
		memcpy(edge_permutation_,temp,12);
		//edge_flip
		temp=multiply_twist(edge_flip_,table_motion_middle_rotation_middle_edge_flip_[x],
			table_motion_middle_rotation_middle_edge_permutation_[x],12,2);
		memcpy(edge_flip_,temp,12);
		//face
		rotate_face(move);
		return;
	}
	else
	{
		//edge
		rotate_edge(move);
		//face
		rotate_face(move);
	}

}

static int get_reverse_move(int move)
{
	assert(move>=0 && move<18);
	return (move/3*3+2-(move%3));
}

void SolveCubeN::solve_face()
{
	//char single_step[8];
	char single_step[16];
	char single_step_length;
	int face_row=(cube_order_/2-1);
	int face_index=0;
	for(int i=1;i<=face_row;i++)
	{
		for(int j=i;j<cube_order_-1-i;j++)
		{

			//int index[2][4];
			//memcpy(index[0],get_4index(i,j,cube_order_),4*sizeof(int));// top right front
			//memcpy(index[1],get_4index_inverse(j,i,cube_order_),4*sizeof(int));//bottom left back
			//for(int k=0;k<24;k++)
			//{
			//	int color=cube->face(k/4)[index[k/12][k%4]];
			//	color=table_color_inverse_[color];
			//	faces_[face_index][k]=color;
			//}
			for(int face_i=5;face_i>=1;face_i--)
			{
				char a[4];//0-23,index
				int a_count=0;
				for(int k=0;k<24;k++)
				{
					if(faces_[face_index][k]==face_i)
					{
						a[a_count]=k;
						a_count++;
					}
				}
				assert(a_count==4);
				int face_state=Permutation::RankSubset(a,4,24);
				single_step_length=0;
				while(table_premove_face_[face_i][face_state]!=-1)
				{
					single_step[single_step_length]=table_premove_face_[face_i][face_state];
					single_step_length++;
					face_state=table_prestate_face_[face_i][face_state];
				}
				assert(single_step_length<=8);

				// apply move to cube data, and revise single_step[]
				if(solve_mode_==1)
				{
					for(int k=0;k<single_step_length;k++)
					{
						if(single_step[k]>=18)
						{
							char *temp = multiply_permutation(faces_[face_index],table_motion_face_inv_[single_step[k]],24);
							memcpy(faces_[face_index],temp,24);
						}
						else
						{
							int reverse_pre_move=get_reverse_move(single_step[k]);
							rotate(reverse_pre_move);
						}
					}

					{
						int x=face_i*4;
						assert(faces_[face_index][x]==face_i);
						assert(faces_[face_index][x+1]==face_i);
						assert(faces_[face_index][x+2]==face_i);
						assert(faces_[face_index][x+3]==face_i);
					}
				}
				else if(solve_mode_==2)
				{
					for(int k=single_step_length-1;k>=0;k--)
					{
						if(single_step[k]<18)
						{
							int reverse_move=get_reverse_move(single_step[k]);
							single_step[single_step_length]=reverse_move;
							single_step_length++;
						}
					}
					assert(single_step_length<=16);
					for(int k=0;k<single_step_length;k++)
					{
						char *temp = multiply_permutation(faces_[face_index],table_motion_face_inv_[single_step[k]],24);
						memcpy(faces_[face_index],temp,24);
					}	
				}
				else
				{
					assert(0);
				}

				// build solution
				Move move;
				for(int k=0;k<single_step_length;k++)
				{
					int x;
					if(single_step[k]<18)
					{
						x=get_reverse_move(single_step[k]);
						move.direction=x/3+10;
						move.index=0;
						/*move.rotation=single_step[k]%3+1;
						if(move.rotation==3)
						{
							move.rotation=-1;
						}*/
						move.rotation=(x%3+2)%4-1;
						solution_.push_back(move);
					}
					else
					{
						x= single_step[k]-18;
						for(int kk=7;kk>=0;kk--)
						{
							move.direction=complex_motion_face_moves[x][kk][0];
							move.index=complex_motion_face_moves[x][kk][1];
							move.rotation=complex_motion_face_moves[x][kk][2];
							if(move.index==-1)
							{
								move.index=i;
							}
							else if(move.index==-2)
							{
								move.index=j;
							}
							if(move.rotation!=2)
							{
								move.rotation=-move.rotation;
							}
							solution_.push_back(move);
						}
					}
				}
			}
			face_index++;
		}
	}
	assert(face_index==face24_num_);
}

char middle_edge_neibor[12][2]=
{
	/*{2,7},
	{3,10},
	{4,18},
	{1,23},
	{15,5},
	{14,12},
	{13,20},
	{16,21},
	{11,6},
	{9,19},
	{24,17},
	{22,8},*/
	{1,6},
	{2,9},
	{3,17},
	{0,22},
	{14,4},
	{13,11},
	{12,19},
	{15,20},
	{10,5},
	{8,18},
	{23,16},
	{21,7},
};

void SolveCubeN::solve_edge()
{
	for(int i=0;i<12;i++)
	{
		int middle_position;
		for(int ii=0;ii<12;ii++)
		{
			if(edge_permutation_[ii]==i)
			{
				middle_position=ii;
				break;
			}
		}

		char target1;
		char target2;
		char target1_to_position;
		char target2_to_position;
		int x=edge_flip_[middle_position];
		target1=middle_edge_neibor[i][0];
		target2=middle_edge_neibor[i][1];
		target1_to_position=middle_edge_neibor[middle_position][0+x];
		target2_to_position=middle_edge_neibor[middle_position][(1+x)%2];
		for(int j=0;j<edge24_num_;j++)
		{
			if(edges_[j][target2_to_position]!=target1)
			{
				solve_edge_sub(target1,target1_to_position,j);
			}
			else
			{
				solve_edge_sub2(target1_to_position,j);
			}
			assert(edges_[j][target1_to_position]!=target2);
			solve_edge_sub(target2,target2_to_position,j);
		}
		


	}
}

void SolveCubeN::solve_edge_sub(char content, char to, int edge24_index)
{
	char single_step[8];//4*2
	char single_step_length;
	char from;
	for(int ii=0;ii<24;ii++)
	{
		if(edges_[edge24_index][ii]==content)
		{
			from=ii;
			break;;
		}
	}
	assert(basic_motion_edge_append[to/4][to%4]!=from);
	single_step_length=0;
	while(table_premove_edge_[to][from]!=-1)
	{
		single_step[single_step_length]=table_premove_edge_[to][from];
		from = table_prestate_edge_[to][from];
		single_step_length++;
	}
	assert(single_step_length<=4);
	// apply move to cube data, and revise single_step[]
	if(solve_mode_==1)
	{
		for(int k=0;k<single_step_length;k++)
		{
			if(single_step[k]>=18)
			{
				assert(k<42);
				char *temp = multiply_permutation(edges_[edge24_index],table_motion_edge_inv_[single_step[k]],24);
				memcpy(edges_[edge24_index],temp,24);
			}
			else
			{
				int reverse_pre_move=get_reverse_move(single_step[k]);
				rotate(reverse_pre_move);
			}
		}
		assert(edges_[edge24_index][to]==content);
	}
	else if(solve_mode_==2)
	{
		for(int k=single_step_length-1;k>=0;k--)
		{
			if(single_step[k]<18)
			{
				int reverse_move=get_reverse_move(single_step[k]);
				single_step[single_step_length]=reverse_move;
				single_step_length++;
			}
		}
		assert(single_step_length<=8);
		for(int k=0;k<single_step_length;k++)
		{
			assert(k<42);
			char *temp = multiply_permutation(edges_[edge24_index],table_motion_edge_inv_[single_step[k]],24);
			memcpy(edges_[edge24_index],temp,24);
		}
	}
	else
	{
		assert(0);
	}

	// build solution
	Move move;
	for(int k=0;k<single_step_length;k++)
	{
		int x;
		if(single_step[k]<18)
		{
			x=get_reverse_move(single_step[k]);
			move.direction=x/3+10;
			move.index=0;
			move.rotation=(x%3+2)%4-1;
			assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
			solution_.push_back(move);
		}
		else
		{
			x= single_step[k]-18;
			assert(x<24);
			for(int kk=11;kk>=0;kk--)
			{
				move.direction=complex_motion_edge_moves[x][kk][0];
				move.index=complex_motion_edge_moves[x][kk][1];
				move.rotation=complex_motion_edge_moves[x][kk][2];
				if(move.index==-1)
				{
					move.index = edge24_index+1;
				}
				if(move.rotation!=2)
				{
					move.rotation=-move.rotation;
				}
				assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
				solution_.push_back(move);
			}
		}
	}
}

void SolveCubeN::solve_edge_sub2(char to, int edge24_index)
{
	int x1=basic_motion_edge_append[to/4][to%4];
	int x2=to;
	char temp=edges_[edge24_index][x1];
	edges_[edge24_index][x1]=edges_[edge24_index][x2];
	edges_[edge24_index][x2]=temp;
	Move move;
	for(int kk=14;kk>=0;kk--)
	{
		move.direction	=complex_motion_edge_moves_special[to][kk][0];
		move.index		=complex_motion_edge_moves_special[to][kk][1];
		move.rotation	=complex_motion_edge_moves_special[to][kk][2];
		if(move.index==-1)
		{
			move.index=edge24_index+1;
		}
		if(move.rotation!=2)
		{
			move.rotation=-move.rotation;
		}
		assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
		solution_.push_back(move);
	}
}

void SolveCubeN::solve_cube3()
{
	SolveCube3::GetInstance()->GetData(corner_permutation_,corner_twist_,edge_permutation_,edge_flip_);
	SolveCube3::GetInstance()->SolveCore();
	int n=SolveCube3::GetInstance()->solve1_length_;
	char* temp=SolveCube3::GetInstance()->solve1_;
	Move move;
	int x;
	for(int i=0;i<n;i++)
	{
		assert(temp[i]>=0&&temp[i]<18);
		x=temp[i];//get_reverse_move(temp[i]);
		move.direction=x/3+10;
		move.index=0;
		move.rotation=(x%3+2)%4-1;
		assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
		solution_.push_back(move);
	}
	n=SolveCube3::GetInstance()->solve2_length_;
	temp=SolveCube3::GetInstance()->solve2_;
	for(int i=0;i<n;i++)
	{
		assert(temp[i]>=0&&temp[i]<18);
		x=temp[i];//get_reverse_move(temp[i]);
		move.direction=x/3+10;
		move.index=0;
		move.rotation=(x%3+2)%4-1;
		assert(move.rotation==-1 || move.rotation==1 || move.rotation==2);
		solution_.push_back(move);
	}
}

void SolveCubeN::SolveCore()
{
	assert(cube_order_>=3);

	//prepare
	if(cube_order_%2==0)
	{
		make_random_middle_edge();
	}

	solution_.clear();
	//face
	solve_face();
	//edge
	solve_edge();
	//solve cube3
	solve_cube3();

	//std::cout<<"Solve Done. Total move (face move) : "<<solution_.size()<<std::endl;
}
void SolveCubeN::Solve()
{
	SolveCore();

	// direction adjust, to graphic description
	for(unsigned int i=0;i<solution_.size();i++)
	{
		int x=solution_.at(i).direction;
		if(x>=10)
		{
			x=x-10;
			if(x==3||x==4||x==5)
			{
				solution_.at(i).direction=6-x;
				if(solution_.at(i).rotation!=2)
				{
					solution_.at(i).rotation=-solution_.at(i).rotation;
				}
			}
			else if(x==0||x==1||x==2)
			{
				solution_.at(i).direction=3-x;
				solution_.at(i).index=cube_order_-1-solution_.at(i).index;
			}
			else
			{
				assert(0);
			}
		}
	}
	//rotation adjust, reverse and to single move
	solution_single_.clear();
	for(unsigned int i=0;i<solution_.size();i++)
	{
		if(solution_.at(i).rotation==2)
		{
			solution_.at(i).rotation=1;
			solution_single_.push_back(solution_.at(i));
			solution_single_.push_back(solution_.at(i));
			solution_.at(i).rotation=2;
		}
		else
		{
			solution_.at(i).rotation=-solution_.at(i).rotation;
			solution_single_.push_back(solution_.at(i));
			assert(solution_.at(i).rotation==-1 || solution_.at(i).rotation==1);
		}
	}
}