#ifndef CUBE_SOLVECUBE3_H_
#define CUBE_SOLVECUBE3_H_

#include "Cube.h"
#include "SolveCube.h"

class SolveCube3: public SolveCube
{
public:
	static SolveCube3 * GetInstance()
	{
		if(instance_==NULL)
		{
			instance_=new SolveCube3;
		}
		return instance_;
	}
private:
	static SolveCube3 * instance_;



public:
	virtual void Init();
	virtual void GetData(Cube * c);
	virtual void Solve();
	virtual void MakeRandomCube(Cube *c);
	virtual void PrintState();

private:
	SolveCube3()
	{
		Init();
	}
	void make_table_corner();
	void make_table_edge();
	void make_table_motion();
	void make_table_transport();
	void make_table_prun();
	void make_table_prun_template(char * name,int n1,int n2, int table1[][18], int table2[][18],char *table_deep,bool skip=false);
	void GetColorData(Cube *cube);
	char get_corner_id(int i,int j,int k);//i belong to Up or Down
	char get_edge_id(int i,int j);//i belong to Up or Down or Front or Back
	char get_corner_id_from_cube(int i,int j,int k);//i belong to Up or Down
	char get_edge_id_from_cube(int i,int j);//i belong to Up or Down or Front or Back
	char get_corner_twist(int i,int j,int k);
	char get_edge_flip(int i,int j);
	char get_corner_twist_from_cube(int i,int j,int k);
	char get_edge_flip_from_cube(int i,int j);
	int	 slice2int(char* a);
	char* slice2int_inverse(int n);
	int  flip2int(char* a);
	char* flip2int_inverse(int n);
	int  corner2int(char* a);
	char* corner2int_inverse(int n);
	int  twist2int(char* a);
	char* twist2int_inverse(int n);
	int  edge8int(char* a);
	char* edge8int_inverse(int n);
	int  edge4int(char* a);
	char* edge4int_inverse(int n);



	
	void TakeMotion(char *p_corner, char *p_edge, char * twist, char * flip, int move);

	static inline bool changed_motion(int m)// for cube3
	{
		//return(j==3||j==5||j==6||j==8||j==12||j==14||j==15||j==17);
		return((m%3!=1) && (m/3%3!=0));
	}

	inline bool valid_pair(int i,int j)
	{
		if(i/3==j/3)
		{
			return false;
		}
		if((j/3-i/3)==3)
		{
			return false;
		}
		return true;
	}
	
	bool valid_twist();
	bool valid_flip();
	bool solvable();
	void Search1(int slice,int twist, int flip,int max_deep, int index);
	void StartSeach2();
	void Search2(int edge4,int corner,int edge8,int max_deep, int index);
	bool IsOrigin();

public:
	int solution[64][3];
	int solution_[64];
	int solution_length;
private:
	//static
	char table_corner_[8];
	char table_corner_inverse_[8];
	char table_edge_[12];
	char table_edge_inverse_[49];
	
	char table_motion_corner[18][8];
	char table_motion_edge[18][12];
	char table_motion_twist[18][8];
	char table_motion_flip[18][12];

	int  table_transport_corner[40320][18];//8!
	int  table_transport_twist[2187][18];//3^7
	int  table_transport_flip[2048][18];//2^11
	int	 table_transport_edge8[40320][18];//8!
	int  table_transport_edge4[24][18];//4!
	int  table_transport_slice[495][18];//12*11*10*9/4!
	
	char table_trun_slice_flip[495*2048];
	char table_trun_slice_twist[495*2187];
	char table_trun_edge4_edge8[24*40320];
	char table_trun_edge4_corner[24*40320];

	//dynamic
	char corner_permutation_[8];
	char edge_permutation_[12];
	char corner_twist_[8];
	char edge_flip_[12];
	char table_color_[6];
	char table_color_inverse_[7];// search color
	char solve1_[32];
	char solve2_[32];
	int  solve1_length_;
	int  solve2_length_;
	bool search1_stop_;
	bool search2_stop_;

//test
private:
	void test();
};


#endif