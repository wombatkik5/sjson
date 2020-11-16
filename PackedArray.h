#pragma once

#ifndef __PACKEDARRAY_H__
#define __PACKEDARRAY_H__

#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>

class PackedArray
{
private:
	uint64_t _size;		 // size in bytes of useful content
	uint64_t _capacity;	 // total size in bytes of dynamic array
	char *_array;	   // array of raw blob data

public:
	// default constructor
	PackedArray(uint64_t size = 0, char *array = nullptr) : _size(size), _capacity(size), _array(array)
	{
	}

	// move constructor
	PackedArray(PackedArray &&other) : _size(other._size), _capacity(other._capacity), _array(other._array)
	{
		other._size = 0;
		other._capacity = 0;
		other._array = nullptr;
	}

	/* Destructor */
	~PackedArray()
	{
		if(_array != nullptr)
		{
			delete[] _array;
		}
	}

	/* Methods */
	template <typename T>
	void append(T &&newValue)
	{
		// compute total size required to store this new element
		uint64_t newTop = _size + sizeof(newValue);
		// double length of the dynamic array until it fits the new element
		while (newTop > _capacity) { doubleCapacity(); }
		// useful typedefs (there's some c++ complication going on here)
		typedef typename std::remove_reference<T>::type NoRefT;
		typedef typename std::add_pointer<NoRefT>::type NoRefTPtr;
		// get a reference to the first array slot available
		T &slot = *((NoRefTPtr) this->operator[](_size));
		// move (rvalue) or copy (lvalue) assign new value to the slot
		slot = std::forward<T>(newValue);
		// update the useful content pointer to new size
		_size = newTop;
	}

	void appendArray(char *valueArray, const uint64_t count)
	{
		for(uint64_t i = 0; i < count; ++i)
		{
			append(valueArray[i]);
		}
	}

	template <typename T>
	const T &get(const uint64_t index)
	{
		// assert we can read a value of type from the useful content area
		assert(index + sizeof(T) <= _size);
		// access the value at position index as being of type T
		// be warned that no guarantee is made that this is actually of type T
		T *value = (T*) this->operator[](index);
		// return the value as being of type T
		return *value;
	}

	char *getArray(const uint64_t index)
	{
		// assert the first element of the array is in the useful content area
		assert(index < _size);
		// it's up to the caller to manage array size and not go out of bounds
		return (char *)this->operator[](index);
	}

	// size of the useful portion of the array
	uint64_t size() const
	{
		return _size;
	}

	// total capacity of the array
	uint64_t capacity() const
	{
		return _capacity;
	}

	char *getRaw() const
	{
		return _array;
	}

	// copy array to an array of exactly the size of useful content
	void shrink()
	{
		char *newArray = new char[_size];
		memcpy(newArray, _array, _size);
		delete[] _array;
		_array = newArray;
		_capacity = _size;
	}

	PackedArray& operator=(PackedArray &&other)
	{
		// Move-assign operator
		if(this != &other)
		{
			if(_array != nullptr)
			{
				delete[] _array;
			}

			_size = other._size;
			_capacity = other._capacity;
			_array = other._array;

			other._size = 0;
			other._capacity = 0;
			other._array = nullptr;
		}
		return *this;
	}

	bool operator==(const PackedArray &r) const
	{
		return this == &r || (_size == r._size && memcmp(_array, r._array, _size) == 0);
	}

	void *operator[](const uint64_t index) const
	{
		assert(index <= _size); // index == size when we append new element
		return _array + index;
	}

private:
	static const uint64_t INITIAL_CAPACITY = 200;

	void doubleCapacity()
	{
		// initialize if this is the first time double is being called
		if(_capacity == 0)
		{
			init();
			return;
		}

		// double previous capacity
		uint64_t newCapacity = 2 * _capacity;
		// copy array to an array twice the capacity
		// do nothing if the dynamic array is already the desired capacity
		if(newCapacity == _capacity)
		{
			return;
		}
		assert(newCapacity >= _size); // ensure enough space for the content

		// allocate new array
		char *newArray = new char[newCapacity];
		// copy useful contents of old array
		memcpy(newArray, _array, _size);
		// deallocate old array
		delete[] _array;
		// assign the newly allocate array
		_array = newArray;
		_capacity = newCapacity;
	}

	void init()
	{
		_array = new char[_capacity = INITIAL_CAPACITY];
	}

};

#endif
