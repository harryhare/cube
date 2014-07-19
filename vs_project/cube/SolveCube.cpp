#include"SolveCube.h"
#include <cassert>
#include <deque>
#include <fstream>

char* SolveCube::multiply_permutation(char *p, char *p_move, int n)
{
	static char c[12];
	for(int i=0;i<n;i++)
	{
		c[i]=p[p_move[i]];
	}
	return c;
}
char* SolveCube::multiply_twist( char *t,char *t_move, char *p_move, int n, int mod)
{
	static char c[12];
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

