#pragma once

#ifndef __SUCCINCTTREE_H__
#define __SUCCINCTTREE_H__

#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/select_support.hpp>

class SuccinctTree
{
private:
	bool isSupportConstructed = false;
	sdsl::bit_vector _bv; // bit vector that describes the DOM tree structure
	sdsl::bp_support_gg<> *bps; //Used for various queries. /* Originally bp_support_g. */
	sdsl::rank_support_v<> *rsOne;
	sdsl::rank_support_v<0> *rsZero;
	sdsl::select_support_mcl<> *ssOne;
	sdsl::select_support_mcl<0> *ssZero;
	uint64_t _counter;

public:
	SuccinctTree()
	{
		_bv.resize(1);
		_counter = 0;
	}

	SuccinctTree(sdsl::bit_vector b): _bv(b)
	{
		_counter = 0;
	}

	//char* to_char_array();

	sdsl::bit_vector getRaw() const
	{
		return _bv;
	}

	uint8_t get(uint64_t loc) const
	{
		assert(loc <= size());
		return _bv[loc];
	}

	uint64_t enclose(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return bps->enclose(id);
	}

	uint64_t excess(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return bps->excess(id);
	}

	uint64_t findClose(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return bps->find_close(id);
	}

	uint64_t findOpen(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return bps->find_open(id);
	}

	uint64_t getCounter()
	{
		return _counter;
	}

	uint64_t rankOne(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return (*rsOne)(id);
	}

	uint64_t rankZero(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return (*rsZero)(id);
	}

	uint64_t selectOne(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return (*ssOne)(id);
	}

	uint64_t selectZero(uint64_t id)
	{
		assert(isSupportConstructed);
		assert(id <= size());
		return (*ssZero)(id);
	}

	uint64_t size() const
	{
		return _bv.size();
	}

	void incCounter(uint64_t count)
	{
		_counter += count;
	}

	void initSupport()
	{
		isSupportConstructed = false;

		bps = new sdsl::bp_support_gg<>(&_bv); /* Originally bp_support_g. */
		rsOne = new sdsl::rank_support_v<>(&_bv);
		rsZero = new sdsl::rank_support_v<0>(&_bv);
		ssOne = new sdsl::select_support_mcl<>(&_bv);
		ssZero = new sdsl::select_support_mcl<0>(&_bv);

		isSupportConstructed = true;
	}

	void resize(uint64_t sz)
	{
		_bv.resize(sz);
	}

	void set(uint64_t loc)
	{
		assert(_counter <= loc);

		while(size() <= loc)
		{
			resize(size() * 2);
		}

		_bv[loc] = 1;
		incCounter(1);
	}

	void setPush(uint64_t loc, uint64_t count)
	{
		incCounter(count);

		while(size() <= _counter)
		{
			resize(size() * 2);
		}

		std::rotate(_bv.begin() + loc, _bv.begin() + _counter - count, _bv.begin() + _counter);

		for(uint64_t i = loc; i < loc + count; i++)
		{
			_bv[i] = 1;
		}
	}

	void shrink()
	{
		resize(_counter);

		initSupport();
	}

	void unsetPush(uint64_t loc, uint64_t count)
	{
		incCounter(count);

		while(size() <= _counter)
		{
			resize(size() * 2);
		}

		std::rotate(_bv.begin() + loc, _bv.begin() + _counter - count, _bv.begin() + _counter);

		for(uint64_t i = loc; i < loc + count; i++)
		{
			_bv[i] = 0;
		}
	}

	bool operator==(const SuccinctTree &rhs) const
	{
		if(_bv.size() != rhs._bv.size())
		{
			return false;
		}

		for(uint64_t i = 0; i < size(); i++)
		{
			if(_bv[i] != rhs._bv[i])
			{
				return false;
			}
		}

		return true;
	}

	bool operator!=(const SuccinctTree &rhs) const
	{
		return !(*this == rhs);
	}
};

#endif
