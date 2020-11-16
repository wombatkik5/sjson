#pragma once

#ifndef __SJSON_H__
#define __SJSON_H__

#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include "BitmapIndex.h"
#include "Csa.h"
#include "half.hpp"
#include "Jval.h"
#include "SuccinctTree.h"
#include "Util.h"

class Sjson
{
public:
	bool isStringCompressed;
	BitmapIndex values;
	Csa stringArray;
	std::string stringValues;
	std::vector<char *> names;
	std::vector<char *> stringOValues;
	uint64_t size;
	SuccinctTree tree;

public:
	/* Constructors and destructor */
	Sjson(char *, bool);
	Sjson(uint64_t s, SuccinctTree &st, std::vector<char *> &nm, BitmapIndex &vl, std::string &sv) : size(s), tree(st), names(nm), values(std::move(vl)), stringValues(sv) {}
	// TODO: free all names and string values on destructor

	/* Operators */
	const Jval &operator[](const uint64_t index) const;
	bool operator==(const Sjson &rhs) const;

	/* Defined in parse_json.cpp. */
	void parseJson(std::ifstream &, std::unordered_map<std::string, uint64_t> &);

	/* Defined in query.cpp. */
	uint64_t queryChild(uint64_t, uint64_t);
	uint64_t queryDegree(uint64_t);
	std::string queryName(uint64_t);
	void queryNames(uint64_t);
	void queryObjValues(uint64_t, std::string);
	uint64_t queryParent(uint64_t);
	uint8_t queryType(uint64_t);
	void queryValue(uint64_t);

	/* Defined in serialize.cpp. */
	template <class T> uint64_t getSize(const T &);
	Sjson deserializeSingle(const std::string &);
	Sjson deserializeMultiple(const std::string &);
	template <class T> void serializeElement(const T &, std::vector<uint8_t> &);
	void serializeSingle(const std::string &);
	void serializeMultiple(const std::string &);

private:
	/* Private helper methods */
	//void loadInfo(Value &d, unordered_map<string, int> &nt, uint nId=MAX_UINT);
	uint64_t resolveNameId(const std::string &n, std::unordered_map<std::string, uint64_t> &nt);
	//void resolveValue(Value &d, uint nameId=MAX_UINT);

	/* Defined in parse_json.cpp. */
	void parseNull(std::ifstream &);
	void parseTrue(std::ifstream &);
	void parseFalse(std::ifstream &);
	unsigned parseHex4(std::ifstream &);
	void parseStringToStream(std::ifstream &, std::deque<char> &);
	void parseString(std::ifstream &, std::unordered_map<std::string, uint64_t> &, bool);
	void parseObject(std::ifstream &, std::unordered_map<std::string, uint64_t> &);
	void parseArray(std::ifstream &, std::unordered_map<std::string, uint64_t> &);
	void parseNumber(std::ifstream &);
	void parseValue(std::ifstream &, std::unordered_map<std::string, uint64_t> &);

	void skipWhitespace(std::ifstream &);
	void skipWhitespaceAndComments(std::ifstream &);

	/* Defined in query.cpp. */
	uint64_t getIdFromP(uint64_t);
	uint64_t getNext(uint64_t);
	uint64_t getPFromId(uint64_t);
	uint64_t getPrev(uint64_t);
};

template <typename T> T* mapToArray(std::map<T, uint64_t> &mmap);

#endif
