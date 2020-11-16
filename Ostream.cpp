#ifndef __OSTREAM_CPP__
#define __OSTREAM_CPP__

#include <iostream>
#include "SuccinctTree.h"

std::ostream &operator<<(std::ostream &o, const SuccinctTree &t)
{
	o << "(" << t.size() << ",";
	for(uint64_t i = 0; i < t.size(); i++)
	{
		o << t.get(i);
	}
	return o<<")";
}

#endif
