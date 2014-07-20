#ifndef CUBE_SOLVECUBE_H_
#define CUBE_SOLVECUBE_H_
#include "Cube.h"

class SolveCube
{
public:
	virtual void Init()=0;
	virtual void GetData(Cube *c)=0;
	virtual void Solve()=0;
	virtual void MakeRandomCube(Cube *c)=0;
	virtual void PrintState()=0;
	int solution_single_length()const
	{
		return solution_single_length_;
	}
	const int*solution_single_move()const
	{
		return solution_single_move_;
	}

protected:
	static char* multiply_permutation(char *p, char *p_move, int n);
	static char* multiply_twist(char *t,char *t_move, char *p_move, int n, int mod);

	static bool load_table(char *name,char* table,int length);
	static void save_table(char *name,char* table,int length);

protected:
	int solution_single_move_[128];
	int solution_single_length_;
};

#endif