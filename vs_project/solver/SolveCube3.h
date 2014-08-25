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
	virtual void GetData(char *corner_permutation,char*corner_twist,char* edge_permutation,char* edge_flip);
	virtual void Solve();
	virtual void SolveCore();
	virtual void MakeRandomCube(Cube *c);
	void MakeRandomCubeCore();
	virtual void PrintState();

private:
	SolveCube3()
	{
		Init();
	}
	void make_table_motion();
	void make_table_transport();
	void make_table_prun();
	void make_table_prun_template(char * name,int n1,int n2, int table1[][18], int table2[][18],char *table_deep,bool skip=false);
	void GetColorData(Cube *cube);

	static int	 slice2int(char* a);
	static char* slice2int_inverse(int n);
	static int  edge8int(char* a);
	static char* edge8int_inverse(int n);
	static int  edge4int(char* a);
	static char* edge4int_inverse(int n);



	
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
	

	bool solvable();
	void Search1(int slice,int twist, int flip,int max_deep, int index);
	void StartSeach2();
	void Search2(int edge4,int corner,int edge8,int max_deep, int index);
	bool IsOrigin();

public:
	int solution[64][3];
	int solution_[64];
	int solution_length;
//private:
public:	
	static char table_motion_corner[18][8];
	static char table_motion_edge[18][12];
	static char table_motion_twist[18][8];
	static char table_motion_flip[18][12];

private:
	static int  table_transport_corner[40320][18];//8!
	static int  table_transport_twist[2187][18];//3^7
	static int  table_transport_flip[2048][18];//2^11
	static int	 table_transport_edge8[40320][18];//8!
	static int  table_transport_edge4[24][18];//4!
	static int  table_transport_slice[495][18];//12*11*10*9/4!
	
	static char table_trun_slice_flip[495*2048];
	static char table_trun_slice_twist[495*2187];
	static char table_trun_edge4_edge8[24*40320];
	static char table_trun_edge4_corner[24*40320];
public:
	//dynamic
	char solve1_[32];
	char solve2_[32];
	int  solve1_length_;
	int  solve2_length_;
private:
	bool search1_stop_;
	bool search2_stop_;

//test
private:
	void test();
};


#endif