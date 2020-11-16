#ifndef __UTIL_CPP__
#define __UTIL_CPP__

#include "Util.h"

void bitvector_to_char_array(char *p, const sdsl::bit_vector &bv)
{
	// parameter array p should be at least of size bitvector_size_in_bytes
	for(uint64_t i = 0; i < bitvector_size_in_bytes(bv); ++i)
	{
		p[i] = 0;
	}
	for(uint64_t i = 0; i < bv.size(); ++i)
	{
		p[i / 8] |= bv[i] << (7 - (i % 8));
	}
}

sdsl::bit_vector char_array_to_bitvector(char *p, uint64_t size_in_bits)
{
	// parameter size_in_bits should be at most 8 * lenght of p
	sdsl::bit_vector bv = sdsl::bit_vector(size_in_bits, 0);
	for (uint64_t i = 0; i < size_in_bits; ++i)
	{
		bv[i] = p[i / 8] & (1 << (7 - (i % 8)));
	}
	return bv;
}

char* string_to_cstr(const std::string &s)
{
	char *p = new char[s.size() + 1];
	strcpy(p, s.c_str());
	return p;
}

char* cstr_copy(const char * const cstr)
{
	if(cstr == nullptr)
	{
		return nullptr;
	}
	char *p = new char[strlen(cstr) + 1];
	strcpy(p, cstr);
	return p;
}

bool intInChar(int num)
{
	return abs(num) <= SCHAR_MAX;
}

bool intInShort(int num)
{
	return num <= SHRT_MAX;
}

bool isMaxUint(uint64_t num)
{
	return num == MAX_UINT;
}

uint64_t jvalByteSize(const Jval &val)
{
	if(!val.hasName())
	{
		// val does not have name
		if(!val.hasValue())
		{
			return sizeof(Jval);
		}
		if(val.isString())
		{
			return sizeof(StringJval);
		}
		if(val.isChar())
		{
			return sizeof(CharJval);
		}
		if(val.isShort())
		{
			return sizeof(ShortJval);
		}
		if(val.isInt())
		{
			return sizeof(IntJval);
		}
		if(val.isLong())
		{
			return sizeof(LongJval);
		}
		if(val.isHalf())
		{
			return sizeof(HalfJval);
		}
		if(val.isFloat())
		{
			return sizeof(FloatJval);
		}
		if(val.isDouble())
		{
			return sizeof(DoubleJval);
		}
		// unknown type (shouldn't happen)
		assert(false);
	}
	// val has name
	if(!val.hasValue())
	{
		return sizeof(NamedJval);
	}
	if(val.isString())
	{
		return sizeof(NamedStringJval);
	}
	if(val.isChar())
	{
		return sizeof(NamedCharJval);
	}
	if(val.isShort())
	{
		return sizeof(NamedShortJval);
	}
	if(val.isInt())
	{
		return sizeof(NamedIntJval);
	}
	if(val.isLong())
	{
		return sizeof(NamedLongJval);
	}
	if(val.isHalf())
	{
		return sizeof(NamedHalfJval);
	}
	if(val.isFloat())
	{
		return sizeof(NamedFloatJval);
	}
	if(val.isDouble())
	{
		return sizeof(NamedDoubleJval);
	}
	// unknown type (shouldn't happen)
	assert(false);
	return 0;
}

void loadJvalBitmapIndex(BitmapIndex &index)
{
	// total size of bitmap corresponds to the size of packed array
	uint64_t byte_size = index._array.size();
	// allocate a bit vector with the appropriate size
	sdsl::bit_vector bitmap(byte_size, 0);

	// initialize counter for number of elements in the bitmap index
	uint64_t size = 0;
	for(uint64_t cur = 0; cur < byte_size;)
	{
		// set a bit to 1 in the beginning of every element
		bitmap[cur] = 1;
		// skip the remanining bits of the current element
		cur += jvalByteSize(index._array.get<Jval>(cur));
		// count the current element
		size++;
	}

	// assign member attributes and load the select structure
	index._size = size;
	index._bitmap = std::move(bitmap);
	index.loadSelect();
}

#endif
