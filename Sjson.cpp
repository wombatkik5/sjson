#ifndef __SJSON_CPP__
#define __SJSON_CPP__

#include "Sjson.h"

Sjson::Sjson(char *fn, bool is)
{
	isStringCompressed = is;

	// Load names table and values packed array
	std::ifstream fs(fn);

	std::unordered_map<std::string, uint64_t> nameTable;

	parseJson(fs, nameTable);
	tree.shrink();
	//loadInfo(d, nameTable);
	// Load values bitmap index
	values.loadBitmap();

	size = values.size();

	// Reduce excess vector capacity
	names.shrink_to_fit();

	if(isStringCompressed)
	{
		stringValues.shrink_to_fit();
		stringArray.Construct(stringValues);
		std::cout << "Compressed from " << stringValues.size() << " bytes to " << stringArray.SizeInByte() << " bytes." << std::endl;
		stringValues.clear();
	}
	else
	{
		stringOValues.shrink_to_fit();
	}
}

uint64_t Sjson::resolveNameId(const std::string &name, std::unordered_map<std::string, uint64_t> &nameTable)
{
	auto it = nameTable.find(name);
	if(it == nameTable.end())
	{
		uint64_t nameId = names.size();
		nameTable[name] = nameId;
		names.push_back(string_to_cstr(name));
		return nameId;
	}
	return it->second;
}

template <typename T> T* mapToArray(std::map<T, uint64_t> &mmap)
{
	// Allocate space for array a
	T *a = new T[mmap.size()];
	// Copy the elements from map to array
	for(auto it = mmap.begin(); it != mmap.end(); ++it)
	{
		a[it->second] = it->first;
	}
	return a;
}

const Jval& Sjson::operator[](const uint64_t index) const
{
	return values.get<Jval>(index);
}

bool Sjson::operator==(const Sjson &rhs) const
{
	if(names.size() != rhs.names.size())
	{
		return false;
	}
	for(uint64_t i = 0; i < names.size(); ++i)
	{
		if(strcmp(names[i], rhs.names[i]))
		{
			return false;
		}
	}
	return size == rhs.size	&& tree == rhs.tree && values == rhs.values;
}

std::ostream &operator<<(std::ostream &o, const Sjson &p)
{
	o<<"Names ";
	for(uint64_t i = 0; i < p.names.size(); i++)
	{
		o<<i<<":'"<<p.names[i]<<"'"; o<<std::endl;
	}
	o<<"Values ";
	for(uint64_t i = 0; i < p.values.size(); i++)
	{
		const Jval &v = p[i];
		o<<":"<<v; if (v.isString()) o<<p.stringValues[v.getStringIndex()];
	}
	return o<<std::endl;
}

#endif
