#pragma once

#ifndef __JVAL_H__
#define __JVAL_H__

#include "half.hpp"
#include "Util.h"

/* Simple Jval */
class Jval
{
public:
	/* Member variables */
	unsigned char _type;

	/* Type static constants */
	static const unsigned char TYPE_NULL = 1;
	static const unsigned char TYPE_OBJECT = 2;
	static const unsigned char TYPE_ARRAY = 3;
	static const unsigned char TYPE_FALSE = 4;
	static const unsigned char TYPE_TRUE = 5;
	static const unsigned char TYPE_STRING = 6;
	static const unsigned char TYPE_CHAR = 7;
	static const unsigned char TYPE_SHORT = 8;
	static const unsigned char TYPE_INT = 9;
	static const unsigned char TYPE_LONG = 10; //None.
	static const unsigned char TYPE_HALF = 11;
	static const unsigned char TYPE_FLOAT = 12;
	static const unsigned char TYPE_DOUBLE = 13; //10.
	/* Named value flag */
	static const unsigned char NAMED  = (1 << 7);

public:
	/* Constructors */
	Jval(unsigned char type) : _type(type) { }
	// Copy constructor (folds as move constructor too because const ref)
	Jval(const Jval &r) : _type(r._type) {}

	/* Static helper methods */
	inline static unsigned char unname(unsigned char type)
	{
		return type & ~NAMED;
	}
	inline static unsigned char doname(unsigned char type)
	{
		return type | NAMED;
	}
	inline static bool hasName(unsigned char type)
	{
		return type & NAMED;
	}
	inline static bool hasValue(unsigned char t)
	{
		return unname(t) >= TYPE_STRING && unname(t) <= TYPE_DOUBLE;
	}

	/* Type check helper member methods */
	inline bool isNull() const
	{
		return type() == TYPE_NULL;
	}
	inline bool isObject() const
	{
		return type() == TYPE_OBJECT;
	}
	inline bool isArray() const
	{
		return type() == TYPE_ARRAY;
	}
	inline bool isFalse() const
	{
		return type() == TYPE_FALSE;
	}
	inline bool isTrue() const
	{
		return type() == TYPE_TRUE;
	}
	inline bool isBoolean() const
	{
		return isTrue() || isFalse();
	}
	inline bool isString() const
	{
		return type() == TYPE_STRING;
	}
	inline bool isChar() const
	{
		return type() == TYPE_CHAR;
	}
	inline bool isShort() const
	{
		return isChar() || type() == TYPE_SHORT;
	}
	inline bool isInt() const
	{
		return isShort() || type() == TYPE_INT;
	}
	inline bool isLong() const
	{
		return isInt() || type() == TYPE_LONG;
	}
	inline bool isHalf() const
	{
		return type() == TYPE_HALF;
	}
	inline bool isFloat() const
	{
		return isHalf() || type() == TYPE_FLOAT;
	}
	inline bool isDouble() const
	{
		return isFloat() || type() == TYPE_DOUBLE;
	}

	/* Member methods */
	inline unsigned char type() const
	{
		return unname(_type);
	}
	uint64_t nameId() const;
	inline bool hasName() const
	{
		return hasName(_type);
	}
	inline bool hasValue() const
	{
		return hasValue(_type);
	}

	/* Data access methods */
	uint64_t getStringIndex() const;
	int8_t getChar() const;
	int16_t getShort() const;
	int32_t getInt() const;
	int64_t getLong() const;
	half_float::half getHalf() const;
	float getFloat() const;
	double getDouble() const;

	/* Operators */
	// copy-assignment operator (folds as move-assignment operator)
	Jval& operator=(const Jval &other)
	{
		_type = other._type;
		return *this;
	}

} __attribute__ ((packed));

std::ostream &operator<<(std::ostream &o, const Jval &v);

/* Simple Named Jval */
class NamedJval : public Jval
{
public:
	/* Member variables */
	uint64_t _nameId;

public:
	/* Constructors */
	NamedJval(unsigned char type, uint64_t nameId) : Jval(doname(type)), _nameId(nameId) { }
	// Copy constructor (folds as move constructor)
	NamedJval(const NamedJval &r) : Jval(r._type), _nameId(r._nameId) { }

	/* Member methods */
	uint64_t nameId()
	{
		return _nameId;
	}

	/* Operators */
	// copy-assignment operator (folds as move-assignment operator)
	NamedJval& operator=(const NamedJval &other)
	{
		_type = other._type;
		_nameId = other._nameId;
		return *this;
	}

} __attribute__ ((packed));


/* Valued Jval */
template <typename T, unsigned char TYPE>
class DataJval : public Jval {
public:
	/* Member variables */
	T _data;

public:
	/* Constructors */
	DataJval(T data) : Jval(TYPE), _data(data) {}
	// Copy constructor (folds as move constructor)
	DataJval(const DataJval<T,TYPE> &r) : Jval(TYPE), _data(r._data) {}

	/* Operators */
	// copy-assignment operator (folds as move-assignment operator)
	DataJval& operator=(const DataJval &other)
	{
		_type = other._type;
		_data = other._data;
		return *this;
	}
} __attribute__ ((packed));


/* Named Valued Jval */
template <typename T, unsigned char TYPE>
class NamedDataJval : public NamedJval
{
public:
	/* Member variables */
	T _data;

public:
	/* Constructors */
	NamedDataJval(uint64_t nameId, T data) : NamedJval(doname(TYPE), nameId),
			_data(data) {}
	// Copy constructor (folds as move constructor)
	NamedDataJval(const NamedDataJval &r) : NamedJval(doname(TYPE), r._nameId),
			_data(r._data) {}

	/* Operators */
	// copy-assignment operator (folds as move-assignment operator)
	NamedDataJval& operator=(const NamedDataJval &other)
	{
		_type = other._type;
		_nameId = other._nameId;
		_data = other._data;
		return *this;
	}
} __attribute__ ((packed));


/* Typedefs and template instantiations */
typedef DataJval<uint64_t,	 Jval::TYPE_STRING> StringJval;
typedef DataJval<char,	 Jval::TYPE_CHAR>	CharJval;
typedef DataJval<int16_t,  Jval::TYPE_SHORT>  ShortJval;
typedef DataJval<int32_t,	 Jval::TYPE_INT>	IntJval;
typedef DataJval<int64_t,	 Jval::TYPE_LONG>	 LongJval;
typedef DataJval<half_float::half, Jval::TYPE_HALF> HalfJval;
typedef DataJval<float, Jval::TYPE_FLOAT> FloatJval;
typedef DataJval<double, Jval::TYPE_DOUBLE> DoubleJval;

typedef NamedDataJval<uint,	  Jval::TYPE_STRING> NamedStringJval;
typedef NamedDataJval<char,	  Jval::TYPE_CHAR>	 NamedCharJval;
typedef NamedDataJval<int16_t,	Jval::TYPE_SHORT>  NamedShortJval;
typedef NamedDataJval<int32_t,	  Jval::TYPE_INT>	 NamedIntJval;
typedef NamedDataJval<int64_t,	  Jval::TYPE_LONG>	  NamedLongJval;
typedef NamedDataJval<half_float::half, Jval::TYPE_HALF> NamedHalfJval;
typedef NamedDataJval<float, Jval::TYPE_FLOAT> NamedFloatJval;
typedef NamedDataJval<double, Jval::TYPE_DOUBLE> NamedDoubleJval;

#endif
