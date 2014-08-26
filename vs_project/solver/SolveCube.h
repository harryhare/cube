#ifndef CUBE_SOLVECUBE_H_
#define CUBE_SOLVECUBE_H_
#include "Cube.h"
#include <vector>

class Move
{
public:
	char direction;
	char rotation;
	int index;
};


class SolveCube
{
public:
	virtual void Init()
	{
		make_table_corner();
		make_table_edge();
	}
	virtual void GetData(Cube *c)=0;
	virtual void Solve()=0;
	virtual void SolveCore()=0;
	virtual void MakeRandomCube(Cube *c)=0;
	virtual void PrintState()=0;
	virtual int solution_single_length()const
	{
		return solution_single_length_;
	}
	virtual const int*solution_single_move()const
	{
		return solution_single_move_;
	}

protected:
	static char* multiply_permutation(char *p, char *p_move, int n);
	static char* multiply_twist(char *t,char *t_move, char *p_move, int n, int mod);

	static bool load_table(char *name,char* table,int length);
	static void save_table(char *name,char* table,int length);

	static void make_table_corner();
	static void make_table_edge();

	static int find_opposite_color(int *corner_color,int target1,int target2);

	static char get_corner_id(int i, int j,int k);//i belong to Up or Down
	char get_corner_id_from_cube(int i,int j,int k);
	static char get_corner_twist(int i, int j,int k);
	char get_corner_twist_from_cube(int i,int j,int k);
	static char get_edge_id(int i,int j);//i belong to Up or Down or Front or Back
	char get_edge_id_from_cube(int i,int j);//i belong to Up or Down or Front or Back
	static char get_edge_flip(int i,int j);
	char get_edge_flip_from_cube(int i,int j);

	bool valid_twist();
	bool valid_flip();

	void make_random_corner();
	void make_random_middle_edge();

	static int  flip2int(char* a);
	static char* flip2int_inverse(int n);
	static int  corner2int(char* a);
	static char* corner2int_inverse(int n);
	static int  twist2int(char* a);
	static char* twist2int_inverse(int n);

	void fill_corner_pointer(Cube*cube);
	void fill_midedge_pointer(Cube*cube);
	void fill_center_pointer(Cube*cube);
	void corner2cube();
	void midedge2cube();
	void center2cube();
	
protected:

	//static 
	static char table_corner_[8];
	static char table_corner_inverse_[8];
	static char table_edge_[12];
	static char table_edge_inverse_[49];

	//dynamic
	char center_[6];//odd cube only
	char corner_permutation_[8];
	char corner_twist_[8];
	char edge_permutation_[12];//odd cube only
	char edge_flip_[12];//odd cube only

	int *corner_pointer_[8][3];
	int *midedge_pointer_[12][2];// odd cube only
	int *center_pointer_[6];// odd cube only
	char table_color_[6];
	char table_color_inverse_[7];//search color

public:
	int solution_single_move_[128];
	int solution_single_length_;

	std::vector<Move> solution_single_;

	
protected:
	//config
	static bool enable_print_state_;
};

#endif