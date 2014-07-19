//control

#ifndef CUBE_CONTROL_H_
#define CUBE_CONTROL_H_

class Control
{
public:
	static Control& GetInstance()
	{
		static Control instance;
		return instance;
	}
	double x()
	{
		return x_;
	}
	double y()
	{
		return y_;
	}
	double z()
	{
		return z_;
	}
	double *matrix()
	{
		return matrix_;
	}
private:
	Control(void);
	~Control(void);

	//camera
	double x_;
	double y_;
	double z_;
	double matrix_[16];
};

#endif