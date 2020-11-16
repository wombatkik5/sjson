#pragma once

#ifndef __CSA_H__
#define __CSA_H__

#include <cassert>
#include <sdsl/suffix_arrays.hpp>

class Csa
{
private:
	bool isConstructed;

public:
	sdsl::csa_sada<> csa;

public:
	Csa()
	{
		isConstructed = false;
	}

	std::string Get(uint64_t begin)
	{
		assert(isConstructed);

		std::string resultString;
		uint64_t currentLocation = begin;
		while(currentLocation < Size())
		{
			std::string tempString = sdsl::extract(csa, currentLocation, currentLocation);
			if(tempString != "$")
			{
				currentLocation++;
				resultString.append(tempString);
			}
			else
			{
				break;
			}
		}

		return resultString;
	}

	std::string Get(uint64_t begin, uint64_t end)
	{
		assert(isConstructed);
		return sdsl::extract(csa, begin, end);
	}

	uint64_t SizeInByte() const
	{
		assert(isConstructed);
		return sdsl::size_in_bytes(csa);
	}

	uint64_t Size()
	{
		assert(isConstructed);
		return csa.size();
	}

	void AddString(std::string newString)
	{
		std::string rawString = Get(0, Size() - 2); //Ignore two null characters.
		rawString.append(newString);
		isConstructed = false;
		Construct(rawString);
		isConstructed = true;
	}

	void Construct(std::string rawString)
	{
		sdsl::construct_im(csa, rawString, 1);
		isConstructed = true;
	}
};

#endif
