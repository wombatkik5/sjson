#ifndef __JVAL_CPP__
#define __JVAL_CPP__

#include "half.hpp"
#include "Jval.h"

/* Member access methods implementations */

uint64_t Jval::nameId() const
{
	return (hasName(_type)) ? ((NamedJval*) this)->nameId() : MAX_UINT;
}

uint64_t Jval::getStringIndex() const
{
	assert(isString());
	if(hasName())
	{
		return ((NamedStringJval*) this)->_data;
	}
	return ((StringJval*) this)->_data;
}

int8_t Jval::getChar() const
{
	assert(isChar());
	if(hasName())
	{
		return ((NamedCharJval*) this)->_data;
	}
	return ((CharJval*) this)->_data;
}

int16_t Jval::getShort() const
{
	assert(isShort());
	if(isChar())
	{
		return getChar();
	}
	if(hasName())
	{
		return ((NamedShortJval*) this)->_data;
	}
	return ((ShortJval*) this)->_data;
}

int32_t Jval::getInt() const
{
	assert(isInt());
	if(isShort())
	{
		return getShort();
	}
	if(hasName())
	{
		return ((NamedIntJval*) this)->_data;
	}
	return ((IntJval*) this)->_data;
}

int64_t Jval::getLong() const
{
	assert(isLong());
	if(isInt())
	{
		return getInt();
	}
	if(isShort())
	{
		return getShort();
	}
	if(hasName())
	{
		return ((NamedLongJval*) this)->_data;
	}
	return ((LongJval*) this)->_data;
}

half_float::half Jval::getHalf() const
{
	assert(isHalf());
	return ((HalfJval *) this)->_data;
}

float Jval::getFloat() const
{
	assert(isFloat());
	if(isHalf())
	{
		return getHalf();
	}
	return ((FloatJval *) this)->_data;
}

double Jval::getDouble() const
{
	assert(isDouble());
	if(hasName())
	{
		return ((NamedDoubleJval*) this)->_data;
	}
	return ((DoubleJval*) this)->_data;
}

/* Output operator */
std::ostream &operator<<(std::ostream &o, const Jval &v)
{
	std::string types[] = {"NULL", "OBJECT", "ARRAY", "FALSE", "TRUE",
		"STRING", "CHAR", "SHORT", "INT", "LONG", "HALF", "FLOAT", "DOUBLE"};
	o << "(" << types[v.type() - 1];
	if(v.hasName())
	{
		o << ",n" << v.nameId();
	}
	if(v.isString())
	{
		o << ",v" << v.getStringIndex();
	}
	if(v.isInt())
	{
		o << ",v" << v.getInt();
	}
	if(v.isLong())
	{
		o << ",v" << v.getLong();
	}
	if(v.isDouble())
	{
		o << ",v" << v.getDouble();
	}
	return o << ")";
}

/* Definition of static variables */

const unsigned char Jval::TYPE_NULL;
const unsigned char Jval::TYPE_OBJECT;
const unsigned char Jval::TYPE_ARRAY;
const unsigned char Jval::TYPE_FALSE;
const unsigned char Jval::TYPE_TRUE;
const unsigned char Jval::TYPE_STRING;
const unsigned char Jval::TYPE_CHAR;
const unsigned char Jval::TYPE_SHORT;
const unsigned char Jval::TYPE_INT;
const unsigned char Jval::TYPE_LONG;
const unsigned char Jval::TYPE_HALF;
const unsigned char Jval::TYPE_FLOAT;
const unsigned char Jval::TYPE_DOUBLE;
const unsigned char Jval::NAMED;


#endif
