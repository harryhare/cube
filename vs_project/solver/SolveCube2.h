#ifndef CUBE_SOLVECUBE2_H_
#define CUBE_SOLVECUBE2_H_

#include "Cube.h"
#include "SolveCube.h"

class SolveCube2 : public SolveCube
{
public:
	static SolveCube2* GetInstance()
	{
		if(instance_==NULL)
		{
			instance_=new SolveCube2;
		}
		return  instance_;
	}
private:
	static SolveCube2 * instance_;
	static int random_style_;

public:
	virtual void Init();
	virtual void GetData(Cube *c);
	virtual void Solve();
	virtual void SolveCore();
	virtual void MakeRandomCube(Cube *c);
	void MakeRandomCubeCore();
	virtual void PrintState();
private:
	SolveCube2()
	{
		Init();
	}
	void make_table_motion();
	void make_table_transport();
	void make_table_prun();
	void make_table_prun_template(int n1,int n2, int table1[][9], int table2[][9],char *table_deep,char*table_pre_move);
	void GetColorData();

	int  permutation2int(char*a);
	char*permutation2int_inverse(int n);
	int  twist2int(char*a);
	char*twist2int_inverse(int n);

	void TakeMotion(char *permutation, char *twist,int move );

	inline bool valid_pair(int i,int j)
	{
		if(i/3==j/3)
		{
			return false;
		}
		return true;
	}
	bool solvable();
	void Search(int permutation, int twist);
	bool IsOrigin();

private:

	char table_motion_permutation_[9][8];
	char table_motion_twist_[9][8];

	int table_transport_corner_[5040][9];
	int table_transport_twist_[729][9];

	char table_trun_corner_twist_[729*5040];
	char table_pre_move[729*5040];

public:
	//dynamic
	char solve_[32];
	char solve_length_;
	
//test
private:
	void test();
};

#endif