#ifndef __QUERY_CPP__
#define __QUERY_CPP__

#include "Sjson.h"

/* Helper functions. */
uint64_t Sjson::getIdFromP(uint64_t p)
{
	return tree.rankZero(p - 1) + 1;
}

uint64_t Sjson::getNext(uint64_t p)
{
	return tree.selectZero(tree.rankZero(p) + 1);
}

uint64_t Sjson::getPFromId(uint64_t id)
{
	return tree.selectZero(id);
}

uint64_t Sjson::getPrev(uint64_t p)
{
	return tree.selectZero(tree.rankZero(p));
}

/* Actual querying functions. */
uint64_t Sjson::queryChild(uint64_t id, uint64_t num)
{
	assert(id < size);

	uint64_t numChild = queryDegree(id);
	if(num > numChild)
	{
		std::cout << "Invalid." << std::endl;
		return id;
	}
	else
	{
		if(id == 0)
		{
			return getIdFromP(tree.findClose(tree.selectZero(tree.rankZero(0) + 1) - num) + 1);
		}
		else
		{
			return getIdFromP(tree.findClose(tree.selectZero(tree.rankZero(tree.selectZero(id) + 1) + 1) - num) + 1);
		}
	}
}

uint64_t Sjson::queryDegree(uint64_t id)
{
	assert(id < size);

	if(id == 0)
	{
		return tree.selectZero(tree.rankZero(0) + 1) - 1;
	}
	else
        {
		return tree.selectZero(tree.rankZero(tree.selectZero(id) + 1) + 1) - tree.selectZero(id) - 1;
	}
}


std::string Sjson::queryName(uint64_t id)
{
	assert(id < size);

	if(values.get<Jval>(id).hasName())
	{
		return std::string(names[values.get<Jval>(id).nameId()]);
	}
	else
	{
		return "";
	}
}

void Sjson::queryNames(uint64_t id)
{
	assert(id < size);

	if(queryType(id) != 2)
	{
		std::cout << "Invalid." << std::endl;
		return;
	}

	for(uint64_t i = 1; i <= queryDegree(id); i++)
	{
		std::cout << queryName(queryChild(id, i)) << std::endl;
	}
}

void Sjson::queryObjValues(uint64_t id, std::string name)
{
	assert(id < size);

	if(queryType(id) != 2)
	{
		std::cout << "Invalid." << std::endl;
		return;
	}

	for(uint64_t i = 1; i <= queryDegree(id); i++)
	{
		if(queryName(queryChild(id, i)) == name)
		{
			queryValue(queryChild(id, i));
		}
	}
}

uint64_t Sjson::queryParent(uint64_t id)
{
	assert(id < size);

	if(id == 0)
	{
		std::cout << "Invalid." << std::endl;
		return id;
	}
	else if(tree.rankZero(tree.findOpen(tree.selectZero(id))) == 0)
	{
		return 0;
	}
	else
	{
		return tree.rankZero(tree.selectZero(tree.rankZero(tree.findOpen(tree.selectZero(id)))) + 1);
	}
}

uint8_t Sjson::queryType(uint64_t id)
{
	assert(id < size);

	if(values.get<Jval>(id).isNull())
	{
		return 1;
	}
	else if(values.get<Jval>(id).isObject())
	{
		return 2;
	}
	else if(values.get<Jval>(id).isArray())
	{
		return 3;
	}
	else if(values.get<Jval>(id).isFalse())
	{
		return 4;
	}
	else if(values.get<Jval>(id).isTrue())
	{
		return 5;
	}
	else if(values.get<Jval>(id).isString())
	{
		return 6;
	}
	else if(values.get<Jval>(id).isLong() || values.get<Jval>(id).isDouble())
	{
		return 7;
	}
	else
	{
		std::cout << "Invalid." << std::endl;
		return 8;
	}
}

void Sjson::queryValue(uint64_t id)
{
	assert(id < size);

	if(values.get<Jval>(id).isString())
	{
		if(!isStringCompressed)
		{
			std::cout << stringOValues[values.get<Jval>(id).getStringIndex()] << std::endl;
		}
		else
		{
			std::cout << stringArray.Get(values.get<Jval>(id).getStringIndex()) << std::endl;
		}
	}
	else if(values.get<Jval>(id).isLong())
	{
		std::cout << values.get<Jval>(id).getLong() << std::endl;
        }
	else if(values.get<Jval>(id).isDouble())
	{
		std::cout << values.get<Jval>(id).getDouble() << std::endl;
	}
	else if(values.get<Jval>(id).isFalse())
	{
		std::cout << "false" << std::endl;
	}
	else if(values.get<Jval>(id).isTrue())
	{
		std::cout << "true" << std::endl;
	}
	else
	{
		std::cout << "Invalid." << std::endl;
	}
}

#endif
