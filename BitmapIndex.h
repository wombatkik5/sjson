#pragma once

#ifndef __BITMAPINDEX_H__
#define __BITMAPINDEX_H__

#include <utility>
#include <vector>
#include <sdsl/bit_vectors.hpp>
#include "PackedArray.h"
#include "Util.h"

class BitmapIndex
{
public:
	uint64_t _size;
	sdsl::bit_vector _bitmap;
	PackedArray _array;
	sdsl::select_support_mcl<1> select;

public:
	/* Constructors */
	// default constructor
	BitmapIndex() : _size(0) {}
	// move constructor
	BitmapIndex(BitmapIndex&&);
	// deserialization constructor
	BitmapIndex(PackedArray &v) : _array(std::move(v)) {}

	/* Destructor */
	~BitmapIndex()
	{
		if(boolBitmap != nullptr)
		{
			delete boolBitmap;
		}
	}

	/* Methods */
	void loadBitmap();
	// initialize the select support structure
	void loadSelect()
	{
		sdsl::util::init_support(select, &_bitmap);
	}
	uint64_t size() const
	{
		return _size;
	}

	template <typename T> void append(T &&elem)
	{
		// append new value to the packed array
		_array.append(std::forward<T>(elem));
		// increment element counter
		_size++;
		// mark the new elemnt position in boolBitmap
		markNewElementOfSize(sizeof(elem));
	}

	template <typename T> T& get(const uint64_t index) const
	{
		// get a reference to the position in the packed array corresponding to
		// the index-th element
		T *value = (T *)this->operator[](index);
		return *value;
	}

	/* Operators */
	BitmapIndex& operator=(BitmapIndex &&); // move-assignment operator
	bool operator==(const BitmapIndex &) const;
	void *operator[](const uint64_t) const;

private:
	std::vector<bool> *boolBitmap = nullptr;
	void markNewElementOfSize(size_t); // mark a new element in boolBitmap
};

std::ostream &operator<<(std::ostream &o, const BitmapIndex &b);

#endif
