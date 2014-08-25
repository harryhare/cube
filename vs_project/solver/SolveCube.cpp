#include "SolveCube.h"
#include "Permutation.h"
#include <cassert>
#include <deque>
#include <fstream>
#include <algorithm>

bool SolveCube::enable_print_state_=false;

char SolveCube::table_corner_[8];
char SolveCube::table_corner_inverse_[8];
char SolveCube::table_edge_[12];
char SolveCube::table_edge_inverse_[49];
static char corner_faces[8][3]=
{
	{0,1,2},
	{0,2,4},
	{0,4,5},
	{0,5,1},
	{3,2,1},
	{3,4,2},
	{3,5,4},
	{3,1,5},
};

static char edge_faces[12][2]=
{
	{0,1},
	{0,2},
	{0,4},
	{0,5},
	{3,1},
	{3,2},
	{3,4},
	{3,5},
	{2,1},
	{2,4},
	{5,4},
	{5,1},
};


char* SolveCube::multiply_permutation(char *p, char *p_move, int n)
{
	static char c[24];
	for(int i=0;i<n;i++)
	{
		c[i]=p[p_move[i]];
	}
	return c;
}
char* SolveCube::multiply_twist( char *t,char *t_move, char *p_move, int n, int mod)
{
	static char c[24];
	for(int i=0;i<n;i++)
	{
		c[i]=(t[p_move[i]]+t_move[i])%mod ;
	}
	return c;
}

bool SolveCube::load_table(char *name,char* table,int length)
{
	std::ifstream in_file;
	in_file.open(name,std::ifstream::binary);
	if(in_file.is_open()==false)
	{
		return false;
	}
	in_file.seekg(0,in_file.end);
	int file_length=in_file.tellg();
	in_file.seekg(0,in_file.beg);
	if(file_length!=length)
	{
		in_file.close();
		return false;
	}
	in_file.read(table,length);
	in_file.close();
	return true;
}
void SolveCube::save_table(char *name,char* table,int length)
{
	std::ofstream out_file(name,std::ofstream::binary);
	assert(out_file.is_open());
	out_file.write(table,length);
	out_file.close();
}


int SolveCube::find_opposite_color(int *corner_color,int target1,int target2)
{
	int target_color=((1<<target1)|(1<<target2));
	int result=0;
	for(int i=1;i<9;i++)
	{
		if((corner_color[i]&target_color)==target_color)
		{
			result = (corner_color[i]&(~target_color));
			break;
		}
	}
	assert(result>0);
	int n=0;
	while((1<<n)<result)
	{
		n++;
	}
	assert((1<<n)==result);
	return n;
}
void SolveCube::make_table_corner()
{
	//table_corner_[0]=007;//000111b;
	//table_corner_[1]=025;//010101b;
	//table_corner_[2]=061;//110001b;
	//table_corner_[3]=043;//100011b;
	//table_corner_[4]=016;//001110b;
	//table_corner_[5]=034;//011100b;
	//table_corner_[6]=070;//111000b;
	//table_corner_[7]=052;//101010b;
	table_corner_[0]=7;//000111b;
	table_corner_[1]=5;//010101b;
	table_corner_[2]=1;//110001b;
	table_corner_[3]=3;//100011b;
	table_corner_[4]=6;//001110b;
	table_corner_[5]=4;//011100b;
	table_corner_[6]=0;//111000b;
	table_corner_[7]=2;//101010b;
	for(int i=0;i<8;i++)
	{
		table_corner_inverse_[table_corner_[i]]=i;
	}
}
void SolveCube::make_table_edge()
{
	table_edge_[0] =003;//000011;
	table_edge_[1] =005;//000101;
	table_edge_[2] =021;//010001;
	table_edge_[3] =041;//100001;


	table_edge_[4] =012;//001010;
	table_edge_[5] =014;//001100;
	table_edge_[6] =030;//011000;
	table_edge_[7] =050;//101000;

	table_edge_[8] =006;//000110;
	table_edge_[9] =024;//010100;
	table_edge_[10]=060;//110000;
	table_edge_[11]=042;//100010;

	for(int i=0;i<sizeof(table_edge_inverse_);i++)
	{
		table_edge_inverse_[i]=-1;
	}
	for(int i=0;i<12;i++)
	{
		table_edge_inverse_[table_edge_[i]]=i;
	}

}




char SolveCube::get_corner_id(int i, int j,int k)
{
	int x=0;
	x |= (1<<i);
	x |= (1<<j);
	x |= (1<<k);
	x &= 007;
	return table_corner_inverse_[x];
}
char SolveCube::get_corner_id_from_cube(int i,int j,int k)
{
	i=table_color_inverse_[i];
	j=table_color_inverse_[j];
	k=table_color_inverse_[k];
	return get_corner_id(i,j,k);
}


char SolveCube::get_corner_twist(int i,int j,int k)
{
	i=i%3;
	j=j%3;
	k=k%3;
	if(i==0)
	{
		return 0;
	}
	if(j==0)
	{
		return 1;
	}
	if(k==0)
	{
		return 2;
	}
	return -1;
}
char SolveCube::get_corner_twist_from_cube(int i,int j,int k)
{
	i=table_color_inverse_[i];
	j=table_color_inverse_[j];
	k=table_color_inverse_[k];
	return get_corner_twist(i,j,k);
}


char SolveCube::get_edge_id(int i,int j)
{
	int x=0;
	x |= (1<<i);
	x |= (1<<j);
	return table_edge_inverse_[x];
}
char SolveCube::get_edge_id_from_cube(int i,int j)
{
	i=table_color_inverse_[i];
	j=table_color_inverse_[j];
	return get_edge_id(i,j);
}

char SolveCube::get_edge_flip(int i,int j)
{
	i=i%3;
	j=j%3;
	if(i==0&&j!=0)
	{
		return 0;
	}
	if(i!=0&&j==0)
	{
		return 1;
	}
	if(i==2&&j==1)
	{
		return 0;
	}
	if(i==1&&j==2)
	{
		return 1;
	}
	return -1;
}
char SolveCube::get_edge_flip_from_cube(int i,int j)
{

	i=table_color_inverse_[i];
	j=table_color_inverse_[j];
	return get_edge_flip(i,j);
}


bool SolveCube::valid_twist()
{
	int sum=0;
	for(int i=0;i<8;i++)
	{
		if(corner_twist_[i]<0 || corner_twist_[i]>2)
		{
			return false;
		}
		sum+=corner_twist_[i];
	}
	return (sum%3==0);
}

bool SolveCube::valid_flip()
{
	int sum=0;
	for(int i=0;i<12;i++)
	{
		if(edge_flip_[i]<0 || edge_flip_[i]>1)
		{
			return false;
		}
		sum+=edge_flip_[i];
	}
	return (sum%2==0);
}

void SolveCube::make_random_corner()
{
	for(int i=0;i<8;i++)
	{
		corner_permutation_[i]=i;
	}
	std::random_shuffle(corner_permutation_,corner_permutation_+8);
	int s1=Permutation::Signature(corner_permutation_,8);
	int s2=Permutation::Signature(edge_permutation_,12);
	/*if(s1!=s2)
	{
		int temp=corner_permutation_[0];
		for(int i=0;i<7;i++)
		{
			corner_permutation_[i]=corner_permutation_[i+1];
		}
		corner_permutation_[7]=temp;
	}
	s1=Permutation::Signature(corner_permutation_,8);
	assert(s1==s2);
*/
	assert(RAND_MAX>10000);
	int twist=rand()%2187;

	char * temp;
	temp=twist2int_inverse(twist);
	memcpy(corner_twist_,temp,8);
}

void SolveCube::make_random_middle_edge()
{
	for(int i=0;i<12;i++)
	{
		edge_permutation_[i]=i;
	}
	std::random_shuffle(edge_permutation_,edge_permutation_+12);
	int s1=Permutation::Signature(corner_permutation_,8);
	int s2=Permutation::Signature(edge_permutation_,12);
	if(s1!=s2)
	{
		int temp=edge_permutation_[0];
		for(int i=0;i<11;i++)
		{
			edge_permutation_[i]=edge_permutation_[i+1];
		}
		edge_permutation_[11]=temp;
	}
	s2=Permutation::Signature(edge_permutation_,12);
	assert(s1==s2);

	assert(RAND_MAX>10000);
	int flip=rand()%2048;

	char *temp;
	temp=flip2int_inverse(flip);
	memcpy(edge_flip_,temp,12);
}


int  SolveCube::corner2int(char* a)
{
	char b[8];
	memcpy(b,a,8);
	return Permutation::RankPermutation(b,8);
}
char* SolveCube::corner2int_inverse(int n)
{
	static char a[8];
	Permutation::UnRankPermutation(n,8,a);
	return a;
}

int  SolveCube::flip2int(char* a)
{
	int x=0;
	for(int i=0;i<11;i++)
	{
		x=(x<<1)+a[i];
	}
	return x;
}
char* SolveCube::flip2int_inverse(int n)
{
	assert(n<2048);
	static char a[12];
	int sum=0;
	for(int i=10;i>=0;i--)
	{
		a[i]= n%2;
		n   = n/2;
		sum+=a[i];
	}
	a[11] = (sum%2);
	return a;
}
int  SolveCube::twist2int(char* a)
{
	int x=0;
	for(int i=0;i<7;i++)
	{
		x=(x*3)+a[i];
	}
	return x;
}
char* SolveCube::twist2int_inverse(int n)
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

void SolveCube::fill_corner_pointer(Cube*cube)
{
	int cube_order_= cube->n();
	int corner_index_0 = 0;
	int corner_index_1 = cube_order_-1;
	int corner_index_2 = cube_order_*(cube_order_-1);
	int corner_index_3 = cube_order_*cube_order_-1;
	corner_pointer_[0][0]=&(cube->top()[corner_index_3])	;corner_pointer_[0][1]=&(cube->right()[corner_index_3]);corner_pointer_[0][2]=&(cube->front()[corner_index_3]);
	corner_pointer_[1][0]=&(cube->top()[corner_index_2])	;corner_pointer_[1][1]=&(cube->front()[corner_index_1]);corner_pointer_[1][2]=&(cube->left()[corner_index_3]);
	corner_pointer_[2][0]=&(cube->top()[corner_index_0])	;corner_pointer_[2][1]=&(cube->left()[corner_index_2]);	corner_pointer_[2][2]=&(cube->back()[corner_index_1]);
	corner_pointer_[3][0]=&(cube->top()[corner_index_1])	;corner_pointer_[3][1]=&(cube->back()[corner_index_3]);	corner_pointer_[3][2]=&(cube->right()[corner_index_2]);
	corner_pointer_[4][0]=&(cube->bottom()[corner_index_3])	;corner_pointer_[4][1]=&(cube->front()[corner_index_2]);corner_pointer_[4][2]=&(cube->right()[corner_index_1]);
	corner_pointer_[5][0]=&(cube->bottom()[corner_index_2])	;corner_pointer_[5][1]=&(cube->left()[corner_index_1]);	corner_pointer_[5][2]=&(cube->front()[corner_index_0]);
	corner_pointer_[6][0]=&(cube->bottom()[corner_index_0])	;corner_pointer_[6][1]=&(cube->back()[corner_index_0]);	corner_pointer_[6][2]=&(cube->left()[corner_index_0]);
	corner_pointer_[7][0]=&(cube->bottom()[corner_index_1])	;corner_pointer_[7][1]=&(cube->right()[corner_index_0]);corner_pointer_[7][2]=&(cube->back()[corner_index_2]);
}
void SolveCube::fill_midedge_pointer(Cube*cube)
{
	int cube_order_=cube->n();
	int edge_index_1 = cube_order_/2;
	int edge_index_3 = (cube_order_/2)*cube_order_;
	int edge_index_5 = cube_order_/2*cube_order_+cube_order_-1;;
	int edge_index_7 = cube_order_*cube_order_-1-cube_order_/2;
	midedge_pointer_[0][0]=&(cube->top()[edge_index_5]);	midedge_pointer_[0][1]=&(cube->right()[edge_index_7]);
	midedge_pointer_[1][0]=&(cube->top()[edge_index_7]);	midedge_pointer_[1][1]=&(cube->front()[edge_index_5]);
	midedge_pointer_[2][0]=&(cube->top()[edge_index_3]);	midedge_pointer_[2][1]=&(cube->left()[edge_index_7]);
	midedge_pointer_[3][0]=&(cube->top()[edge_index_1]);	midedge_pointer_[3][1]=&(cube->back()[edge_index_5]);
	midedge_pointer_[4][0]=&(cube->bottom()[edge_index_5]);	midedge_pointer_[4][1]=&(cube->right()[edge_index_1]);
	midedge_pointer_[5][0]=&(cube->bottom()[edge_index_7]);	midedge_pointer_[5][1]=&(cube->front()[edge_index_3]);
	midedge_pointer_[6][0]=&(cube->bottom()[edge_index_3]);	midedge_pointer_[6][1]=&(cube->left()[edge_index_1]);
	midedge_pointer_[7][0]=&(cube->bottom()[edge_index_1]);	midedge_pointer_[7][1]=&(cube->back()[edge_index_3]);
	midedge_pointer_[8][0]=&(cube->front()[edge_index_7]);	midedge_pointer_[8][1]=&(cube->right()[edge_index_5]);
	midedge_pointer_[9][0]=&(cube->front()[edge_index_1]);	midedge_pointer_[9][1]=&(cube->left()[edge_index_5]);
	midedge_pointer_[10][0]=&(cube->back()[edge_index_1]);	midedge_pointer_[10][1]=&(cube->left()[edge_index_3]);
	midedge_pointer_[11][0]=&(cube->back()[edge_index_7]);	midedge_pointer_[11][1]=&(cube->right()[edge_index_3]);
}
void SolveCube::fill_center_pointer(Cube*cube)
{
	int cube_order=cube->n();
	int index=cube_order*cube_order/2;
	for(int i=0;i<6;i++)
	{
		center_pointer_[i]=&(cube->face(i)[index]);
	}
}

void SolveCube::corner2cube()
{
	int t;
	int p;
	for(int i=0;i<8;i++)
	{
		p=corner_permutation_[i];
		t=corner_twist_[i];
		t=(3-t)%3;//just to unify with solve()
		*(corner_pointer_[i][0])=table_color_[corner_faces[p][(0+t)%3]];
		*(corner_pointer_[i][1])=table_color_[corner_faces[p][(1+t)%3]];
		*(corner_pointer_[i][2])=table_color_[corner_faces[p][(2+t)%3]];
	}
}

void SolveCube::midedge2cube()
{
	int t;
	int p;
	for(int i=0;i<12;i++)
	{
		p=edge_permutation_[i];
		t=edge_flip_[i];
		*(midedge_pointer_[i][0])=table_color_[edge_faces[p][(0+t)%2]];
		*(midedge_pointer_[i][1])=table_color_[edge_faces[p][(1+t)%2]];
	}
}
void SolveCube::center2cube()
{
	int x;
	for(int i=0;i<6;i++)
	{
		x=center_[i];
		*(center_pointer_[i])=table_color_[x];
	}
}