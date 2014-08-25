#ifndef CUBE_SOLVECUBEN_H_
#define CUBE_SOLVECUBEN_H_

#include "Cube.h"
#include "SolveCube.h"
#include <vector>


class SolveCubeN : public SolveCube
{
public:
	static SolveCubeN*GetInstance()
	{
		if(instance_==NULL)
		{
			instance_=new SolveCubeN;
		}
		return instance_;
	}
private:
	static SolveCubeN * instance_;
	


public:
	virtual void Init();
	virtual void GetData(Cube *c);
	virtual void Solve();
	virtual void SolveCore();
	virtual void MakeRandomCube(Cube *c);
	void MakeRandomCubeCore(int n);
	virtual void PrintState();
	
	//access
	void SetSolveMode(int mode)
	{
		solve_mode_=mode;
	}
	int solve_mode()
	{
		return solve_mode_;
	}

private:
	SolveCubeN()
	{
		Init();
		faces_=NULL;
		edges_=NULL;
		face24_num_=0;
		edge24_num_=0;

	}
	~SolveCubeN()
	{
		if(faces_!=NULL)
		{
			free(faces_);
		}
		if(edges_!=NULL)
		{
			free(edges_);
		}
	}
	void make_table_edgeN();//different to cube3
	void make_table_motion();
	void make_table_transport();
	void make_table_trun();
	void get_color_data(Cube *cube);
	void get_color_data_odd(Cube*cube);
	void get_color_data_even(Cube*cube);
	char get_edge_flip();
	char face2int();
	char edge2int();
	
	bool solvable();
	bool valid_edgeN();
	bool valid_face();

	void make_random_face();
	void make_random_edgeN();

	void rotate(int move);//0-17
	void rotate(Move move);//arbitrary
	void rotate_face(Move move);//arbitrary,except face move
	void rotate_edge(Move move);//arbitrary,except face move

	void solve_face();
	void solve_edge();
	void solve_edge_sub(char content, char to, int edge24_index);
	void solve_edge_sub2(char to, int edge24_index);//special 
	void solve_cube3();
public:
	std::vector<Move> solution_;

	
private:
	static char table_edge_id2color_[24];
	static char table_edge_color2id_[36];// only 24 valid
	
	typedef char (*Array24)[24];
	int cube_order_;
	int face24_num_;
	int edge24_num_;
	Array24 edges_;
	Array24 faces_;
	static char table_motion_face_append_[18][3][24];//arbitrary move, except face move
	static char table_motion_face_append_inv_[18][3][24];//arbitrary move,except face move
	static char table_motion_edge_append_[18][24];//arbitrary move, except face move
	static char table_motion_edge_append_inv_[18][24];//arbitrary move,except face move

	static char table_motion_middle_rotation_center_[18][6];
	static char table_motion_middle_rotation_middle_edge_permutation_[18][12];
	static char table_motion_middle_rotation_middle_edge_flip_[18][12];
	static char table_motion_face_[66][24];
	static char table_motion_edge_[66][24];
	static char table_motion_face_inv_[66][24];
	static char table_motion_edge_inv_[66][24];

	static int table_transport_face_[23*22*21][66];//24*23*22*21/24
	static int table_transport_3face_[495][66];//useless
	static int table_transport_edge_[24][66];// [24][18+24]
	

	static char table_depth_face_[6][23*22*21];
	static char table_premove_face_[6][23*22*21];
	static int  table_prestate_face_[6][23*22*21];
	static char table_depth_edge_[24][24];
	static char table_premove_edge_[24][24];
	static int  table_prestate_edge_[24][24];
	static char table_depth_3face_[245025];//12!/24^3 useless
	static char table_premove_3face[245025];//useless
	static int  table_prestate_3face[245025];//useless

	/* 1, solution of less steps, cost more time
	 * 2, solution of more steps, cost less time
	 */
	static int solve_mode_;

private:
	void test();
};

#endif