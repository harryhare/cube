#ifndef CUBE_PERMUTATION_H_
#define CUBE_PERMUTATION_H_

class Permutation
{
public:
	static const int FACTORIAL_LENGTH;//notice the integer range 
	static int factorial[];
	static int binomial[][25];
	static int permutation_[][25];// some may be too large ...
	static void Init();
	static bool IsPermutation(char *a,int n);

	//follow functions do not check legality of input
	static int  Signature(char *a,int n);
	static int RankPermutation(char *a, int n);
	static void UnRankPermutation(int rank, int n, char* a);
	static int RankSubset(char *a, int n_sub, int n_total);
	static void UnRankSubset(int rank, int n_sub, int n_total, char *a);
	static int RankPermutation(char *a, int n_sub,int n_total);
	static void UnRankPermutation(int rank, int n_sub, int n_total,char* a);

	//test
	static void test();
private:
	static bool init_done_;
};


#endif