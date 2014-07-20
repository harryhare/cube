#include <iostream>

#define DEBUG_OUT(str) //{std::cout<<"@"<<__LINE__<<":"<<(str)<<std::endl;}

#define SAFE_DELETE_ARRAY(a) \
	if (a != NULL)\
	{\
		delete[]a;\
		a = NULL;\
	}

#define SAFE_DELETE(a) \
	if (a != NULL)\
	{\
		delete a;\
		a = NULL;\
	}