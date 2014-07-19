#ifndef CUBE_H_
#define CUBE_H_

class Cube
{
	enum SurfaceName
	{
		FRONT,
		RIGHT,
		TOP,
		BACK,
		LEFT,
		BOTTOM
	};
	//enum DirectionName
	//{
	//	HORIZONTAL,
	//	VERTICAL
	//};
	enum RotateAxis
	{
		FRONT_BACK,
		RIGHT_LEFT,
		TOP_BOTTOM
	};
	enum StepName
	{//clockwise
		ZERO_STEP,
		ONE_STEP,
		TWO_STEP,
		THREE_STEP
	};


public:
	Cube(int n);
	~Cube(void);
	void Rotate(RotateAxis a,int n,int step);
	bool Check();

private:
	//void Convert(int &i, SurfaceName s, int x, int y,int z);
	void Convert(int &i, RotateAxis axis,SurfaceName surface, int n, int variable);
	void RotateSurface(SurfaceName s,int step);
	void RotateSideface(RotateAxis a,int n,int step);
	StepName GetStepName(int step);
	static const int SURFACE_NUM = 6;
	static const int ROTATE_CYCLE = 4;
	static const int ROTATE_ARRAY[3][4];
	char* surface_[SURFACE_NUM];
	int n_;
};

#endif