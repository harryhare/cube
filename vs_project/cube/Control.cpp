#include "Control.h"

#include <cmath>

Control::Control(void)
{
	x_ = 2.0*sqrt(1.0/3);
	y_ = 2.0*sqrt(1.0/3);
	z_ = 2.0*sqrt(1.0/3);
}

Control::~Control(void)
{
}
