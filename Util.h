#pragma once

#ifndef __UTIL_H__
#define __UTIL_H__

#include <climits>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <sdsl/bit_vectors.hpp>
#include "BitmapIndex.h"
#include "Jval.h"

class BitmapIndex;
class Jval;

static const uint64_t MAX_UINT = std::numeric_limits<uint64_t>::max();

inline uint64_t bit_size_to_bytes(const size_t bit_size)
{
	return (bit_size + 7) / 8;
}
inline uint64_t bitvector_size_in_bytes(const sdsl::bit_vector &bv)
{
	return std::ceil(bv.size() / 8.0);
}
void bitvector_to_char_array(char *p, const sdsl::bit_vector &bv);
sdsl::bit_vector char_array_to_bitvector(char *p, uint64_t size_in_bits);

char *string_to_cstr(const std::string &);
char *cstr_copy(const char *const);

bool intInChar(int);
bool intInShort(int);

bool isMaxUint(uint64_t);

uint64_t jvalByteSize(const Jval &);
void loadJvalBitmapIndex(BitmapIndex &);

#endif
