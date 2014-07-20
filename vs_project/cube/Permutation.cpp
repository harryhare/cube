#include "Permutation.h"
#include <cassert>
#include <iostream>
const int Permutation::FACTORIAL_LENGTH=13;//need to notice the integer range 
int Permutation::factorial[FACTORIAL_LENGTH];
int Permutation::binomial[FACTORIAL_LENGTH][FACTORIAL_LENGTH];
bool Permutation::init_done_=false;

void Permutation::Init()
{
	if(init_done_==true)
	{
		return;
	}
	factorial[0]=1;
	for(int i=1;i<FACTORIAL_LENGTH;i++)
	{
		factorial[i]=factorial[i-1]*i;
	}
	
	binomial[0][0]=1;
	for(int i=1;i<FACTORIAL_LENGTH;i++)
	{
		binomial[i][0]=1;
		binomial[i][i]=1;
		for(int j=1;j<i;j++)
		{
			binomial[i][j]=binomial[i-1][j-1]+binomial[i-1][j];
		}
	}
	init_done_=true;
}

bool Permutation::IsPermutation(char *a,int n)
{
	int x=0;
	for(int i=0;i<n;i++)
	{
		x |= (1<<a[i]);
	}
	return x==((1<<n)-1);
}

int Permutation::Signature(char *a,int n)
{
	int count=0;
	for(int i=1;i<n;i++)
	{
		for(int j=0;j<i;j++)
		{
			if(a[j]>a[i])
			{
				count++;
			}
		}
	}
	if(count%2)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

int Permutation::RankPermutation(char *a, int n)
{
	if(n==1)
	{
		assert(a[0]==0);
		return 0;
	}
	int x=0;
	x+=factorial[n-1]*a[0];
	for(int i=1;i<n;i++)
	{
		if(a[i]>a[0])
		{
			a[i] -= 1;
		}
	}
	x+=RankPermutation(a+1,n-1);
	return x;
}
void Permutation::UnRankPermutation(int rank,int n, char *a)
{
	if(n==1)
	{
		a[0]=0;
		return;
	}

	a[0] = rank/factorial[n-1];
	rank %= factorial[n-1];
	UnRankPermutation(rank, n-1, a+1);
	for(int j=1;j<n;j++)
	{
		if(a[j]>=a[0])
		{
			a[j]+=1;
		}
	}
}



int Permutation::RankSubset(char *a, int n_sub, int n_total)
{
	assert(n_sub<=n_total);
	if(n_sub==1)
	{
		return a[0];
	}
	int x=0;
	for(int i=0;i<a[0];i++)
	{
		x+=binomial[n_total-i-1][n_sub-1];
	}
	for(int i=1;i<n_sub;i++)
	{
		a[i] -= (a[0]+1);
	}
	assert(n_total>a[0]+1);
	x+=RankSubset(a+1,n_sub-1,n_total-a[0]-1);
	return x;
}

void Permutation::UnRankSubset(int rank, int n_sub, int n_total, char *a)
{
	if(n_sub==1)
	{
		a[0]=rank;
		return;
	}

	for(int i=0;i<=n_total-n_sub;i++)
	{
		if(rank >=binomial[n_total-i-1][n_sub-1])
		{
			rank-=binomial[n_total-i-1][n_sub-1];
		}
		else
		{
			a[0]=i;
			break;
		}
	}
	assert(n_total>a[0]+1);
	UnRankSubset(rank, n_sub-1,n_total-a[0]-1, a+1);
	for(int j=1;j<n_sub;j++)
	{
		a[j]+=a[0]+1;
	}
}

void Permutation::test()
{
	char a[12];

	/*
	for(int j=1;j<=12;j++)
	{
		for(int i=0;i<factorial[j];i++)
		{
			UnRankPermutation(i,j,a);
			assert(RankPermutation(a,j)==i);
		}
	}
	*/
	for(int i=1;i<=12;i++)
	{
		for(int sub=1;sub<=i;sub++)
		{
			for(int r=0;r<binomial[i][sub];r++)
			{
				UnRankSubset(r,sub,i,a);
				assert(RankSubset(a,sub,i)==r);
			}
		}
	}
	char b[]={1,2,3};
	assert(IsPermutation(b,3)==false);
	char b1[]={2,1,0};
	assert(IsPermutation(b1,3)==true);
	char b2[]={0,1,2,3};
	assert(IsPermutation(b2,4)==true);
	assert(IsPermutation(b2,3)==true);
	char b3[]={0,0,1,2};
	assert(IsPermutation(b3,4)==false);
	assert(IsPermutation(b3,3)==false);

	char c1[]={1,2,3};
	assert(Signature(c1,3)==1);
	char c2[]={1,3,2,4};
	assert(Signature(c2,4)==-1);
	assert(Signature(c2,3)==-1);
	char c3[]={5,4,3,2,1};
	assert(Signature(c3,5)==1);
	assert(Signature(c3,4)==1);
	assert(Signature(c3,3)==-1);
	assert(Signature(c3,2)==-1);
	assert(Signature(c3,1)==1);
}
