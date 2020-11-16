#ifndef __BITMAPINDEX_CPP__
#define __BITMAPINDEX_CPP__

#include "BitmapIndex.h"

BitmapIndex::BitmapIndex(BitmapIndex &&other) : _size(other._size),
		_bitmap(other._bitmap), _array(std::move(other._array)), select(other.select)
{
	other._size = 0;
	_array.shrink();
}

void BitmapIndex::loadBitmap()
{
	// reduce any eventual excess packed array capacity
	_array.shrink();
	// initialize a bit vector of size boolBitmap with zeroes
	_bitmap = sdsl::bit_vector(boolBitmap->size(), 0);
	// set bits to '1' in bitmap whenever boolBitmap is true
	for(uint64_t i = 0; i < boolBitmap->size(); i++)
	{
		if(boolBitmap->operator[](i))
		{
			_bitmap[i] = 1;
		}
	}
	// deallocate boolBitmap
	delete boolBitmap;
	boolBitmap = nullptr;

	// initialize select support structure
	loadSelect();
}

BitmapIndex& BitmapIndex::operator=(BitmapIndex &&other)
{
	if(this != &other)
	{
		_size = other._size;
		_bitmap = other._bitmap;
		_array = std::move(other._array);
		select = other.select;

		other._size = 0;
	}
	return *this;
}

bool BitmapIndex::operator==(const BitmapIndex &rhs) const
{
	return this == &rhs ||
		(_size == rhs._size && _bitmap == rhs._bitmap && _array == rhs._array);
}

void* BitmapIndex::operator[](const uint64_t index) const
{
	// select(k) => position of k-th element, where k in [1, _size]
	assert(index < _size);
	// get position of index-th element
	uint64_t position = select(index + 1); // index is 0-based so we add 1
	// return a pointer to the beginning of the index-th element
	return _array[position];
}

void BitmapIndex::markNewElementOfSize(uint64_t elemSize)
{
	// allocate a new boolean bitmap the first time an element is marked
	if(boolBitmap == nullptr)
	{
		boolBitmap = new std::vector<bool>();
	}

	// mark a new element in the boolean bit vector
	boolBitmap->push_back(true); // add a '1' bit to mark beginning of element
	for(uint64_t i = 1; i < elemSize; ++i)
	{
		boolBitmap->push_back(false); // fill remaining space with '0' bits
	}
}

std::ostream &operator<<(std::ostream &o, const BitmapIndex &b) {
	return o << "(" << b._size << "," << b._bitmap << ")";
	// o<<"{";
	// for (auto it : b._array) o<<"'"<<it<<"',";
	// o<<"}";
	// return o<<endl;
}

#endif
