#ifndef __SERIALIZE_CPP__
#define __SERIALIZE_CPP__

#include "Csa.h"
#include "PackedArray.h"
#include "Sjson.h"
#include "Util.h"
#include <cstring>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

typedef std::vector<uint8_t> StreamType;

namespace detail
{
	template <class T>
	struct getSizer;

	template <>
	struct getSizer<std::string>
	{
		static uint64_t value(const std::string &obj)
		{
			return obj.length() * sizeof(uint8_t) + 1;
		}
	};

	template <>
	struct getSizer<char *>
	{
		static uint64_t value(char * const &obj)
		{
			return strlen(obj) + 1;
		}
	};

	template <>
	struct getSizer<sdsl::bit_vector>
	{
		static uint64_t value(const sdsl::bit_vector &obj)
		{
			return bitvector_size_in_bytes(obj); //WHERE?
		}
	};

	template <>
	struct getSizer<SuccinctTree>
	{
		static uint64_t value(const SuccinctTree &obj)
		{
			// return obj.size_in_bytes();
			//return getSizer(obj.bv);
			return bitvector_size_in_bytes(obj.getRaw());
		}
	};

	template <>
	struct getSizer<PackedArray>
	{
		static uint64_t value(const PackedArray &obj)
		{
			// size and blob array
			return sizeof(obj.size()) + obj.size();
		}
	};

	template <>
	struct getSizer<BitmapIndex>
	{
		static uint64_t value(const BitmapIndex &obj)
		{
			// series of elements (packed array only)
			return getSizer<PackedArray>::value(obj._array);
		}
	};

	template <>
	struct getSizer<Csa>
	{
		static uint64_t value(const Csa &obj)
		{
			return obj.SizeInByte();
		}
	};

	template <class T>
	struct getSizer<std::vector<T> >
	{
		static uint64_t value(const std::vector<T> &obj)
		{
			uint64_t s = 0;
			for(auto it = obj.begin(); it != obj.end(); it++)
			{
				s += getSizer<T>::value(*it);
			}
			return s;
		}
	};

	template <class T>
	struct getSizer
	{
		static uint64_t value(const T &obj)
		{
			return sizeof(T);
		}
	};
} // end detail namespace

template <class T>
uint64_t Sjson::getSize(const T &obj)
{
	return detail::getSizer<T>::value(obj);
}

template uint64_t Sjson::getSize<sdsl::bit_vector>(const sdsl::bit_vector &);
template uint64_t Sjson::getSize<SuccinctTree>(const SuccinctTree &);
template uint64_t Sjson::getSize<PackedArray>(const PackedArray &);
template uint64_t Sjson::getSize<BitmapIndex>(const BitmapIndex &);
template uint64_t Sjson::getSize<Csa>(const Csa &);
template uint64_t Sjson::getSize<std::vector<std::string> >(const std::vector<std::string> &);
template uint64_t Sjson::getSize<std::vector<char *> >(const std::vector<char *> &);
template uint64_t Sjson::getSize<std::vector<uint64_t> >(const std::vector<uint64_t> &);
template uint64_t Sjson::getSize<std::string>(const std::string &);
template uint64_t Sjson::getSize<char *>(char * const &);
template uint64_t Sjson::getSize<uint64_t>(const uint64_t &);
template uint64_t Sjson::getSize<double>(const double &);

namespace detail
{
	template <class T>
	struct serializeHelper;

	template <class T>
	void serializer(const T &obj, StreamType::iterator &);

	void serializeArray(char *a, uint64_t size, StreamType::iterator &res) {
		for (uint64_t i = 0; i < size; ++i) serializer(a[i], res);
	}

	template <>
	struct serializeHelper<std::string> {
		static void apply(const std::string &obj, StreamType::iterator &res) {
			// store each char of the string
			for(const auto &cur : obj) { serializer(cur, res); }
			// store the null character in the end
			serializer('\0', res);
		}
	};

	template <>
	struct serializeHelper<char *> {
		static void apply(char *const &obj, StreamType::iterator &res) {
			// store each char of the char array
			for (char *cur = obj; *cur; cur++) { serializer(*cur, res); }
			// store the null character in the end
			serializer('\0', res);
		}
	};

	template <>
	struct serializeHelper<sdsl::bit_vector> {
		static void apply(const sdsl::bit_vector &obj, StreamType::iterator &res) {
			uint64_t size = bitvector_size_in_bytes(obj);
			char *p = new char[size];
			bitvector_to_char_array(p, obj);
			serializeArray(p, size, res);
			delete[] p;
		}
	};

	template <>
	struct serializeHelper<Csa> {
		static void apply(const Csa &obj, StreamType::iterator &res)
		{
			uint64_t size = obj.SizeInByte();
			char *p = new char[size];
			std::stringstream pstr;
			pstr.rdbuf()->pubsetbuf(p, sizeof(p));
			obj.csa.serialize(pstr);
			serializeArray(p, size, res);
			std::cout << "Done." << std::endl;
			delete[] p;
		}
	};

	template <>
	struct serializeHelper<SuccinctTree> {
		static void apply(const SuccinctTree &obj, StreamType::iterator &res) {
			// char *p = obj.to_char_array();
			// int size = obj.size_in_bytes();
			// serialize_array(p, size, res);
			serializer(obj.getRaw(), res);
		}
	};

	template <>
	struct serializeHelper<PackedArray> {
		static void apply(const PackedArray &obj, StreamType::iterator &res) {
			// serialize number of bytes
			serializer(obj.size(), res);
			// serialize byte array
			serializeArray(obj.getRaw(), obj.size(), res);
		}
	};

	template <>
	struct serializeHelper<BitmapIndex> {
		static void apply(const BitmapIndex &obj, StreamType::iterator &res) {
			// serialize values
			serializer(obj._array, res);
		}
	};

	template <class T>
	struct serializeHelper<std::vector<T> > {
		static void apply(const std::vector<T> &obj, StreamType::iterator &res) {
			for (auto it = obj.begin(); it != obj.end(); it++)
				serializer(*it, res);
		}
	};

	template <class T>
	struct serializeHelper {
		static void apply(const T& obj, StreamType::iterator& res) {
			const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&obj);
			std::copy(ptr, ptr + sizeof(T), res);
			res += sizeof(T);
		}
	};

	template <class T>
	void serializer(const T &obj, StreamType::iterator &res) {
		serializeHelper<T>::apply(obj, res);
	}

} // end detail namespace

template <class T>
void Sjson::serializeElement(const T &obj, StreamType &res) {
	uint64_t offset = res.size();
	uint64_t size = getSize(obj);
	res.resize(res.size() + size);

	StreamType::iterator it = res.begin() + offset;
	detail::serializer(obj, it);
	// cout << "exp: " << offset + size << " | got:" << it - res.begin() << endl;
	assert(res.begin() + offset + size == it);
}

template void Sjson::serializeElement<sdsl::bit_vector>(const sdsl::bit_vector &, StreamType &);
template void Sjson::serializeElement<SuccinctTree>(const SuccinctTree &, StreamType &);
template void Sjson::serializeElement<BitmapIndex>(const BitmapIndex &, StreamType &);
template void Sjson::serializeElement<std::vector<std::string> >(const std::vector<std::string> &, StreamType &);
template void Sjson::serializeElement<std::vector<char *> >(const std::vector<char *> &, StreamType &);
template void Sjson::serializeElement<std::vector<uint64_t> >(const std::vector<uint64_t> &, StreamType &);
template void Sjson::serializeElement<std::string>(const std::string &, StreamType &);
template void Sjson::serializeElement<Csa>(const Csa &, StreamType &);
template void Sjson::serializeElement<char *>(char * const &, StreamType &);
template void Sjson::serializeElement<uint64_t>(const uint64_t &, StreamType &);
template void Sjson::serializeElement<double>(const double &, StreamType &);

/* Originally save_to_file. TODO: Size of everything not stored yet? */
void Sjson::serializeSingle(const std::string &filename)
{
	StreamType res;
	//serializeElement(res);

	uint64_t offset = res.size();
	// size of header
	uint64_t s = sizeof(uint64_t);
	// size of tree
	s += getSize(tree);
	// size of names table
	s += sizeof(uint64_t);
	s += getSize(names);
	// size of values BitmapIndex
	s += getSize(values);
	// size of stringValues array
	s += sizeof(uint64_t);
	if(!isStringCompressed)
	{
		s += getSize(stringOValues);
	}
	else
	{
		s += getSize(stringArray);
	}
	res.resize(res.size() + s);

	StreamType::iterator it = res.begin();
	//detail::serializer(obj, it);

	// store header
	detail::serializer(s, it);
	// store the tree
	detail::serializer(tree, it);
	// store name table
	detail::serializer(names.size(), it);
	detail::serializer(names, it);
	// store bitmap indexes for values
	detail::serializer(values, it);
	// store stringValues array
	if(!isStringCompressed)
	{
		detail::serializer(stringOValues.size(), it);
		detail::serializer(stringOValues, it);
	}
	else
	{
		detail::serializer(stringArray.SizeInByte(), it);
		detail::serializer(stringArray, it);
	}

	assert(res.begin() + s == it);

	//save_to_file(res, filename);

	std::ofstream ofs(filename, std::ios::binary);
	ofs.write((char *)&res[0], res.size());
	ofs.close();
}

/* Originally save_to_file_split. */
void Sjson::serializeMultiple(const std::string &filename) {
	const uint64_t nsplit = 5;
	std::string splitnames[] = {"_header", "_tree", "_names", "_values", "_strvalues"};

	StreamType res[nsplit];
	serializeElement(size, res[0]); // header (size)
	serializeElement(names.size(), res[0]); // header (name table size)
	serializeElement(stringValues.size(), res[0]); // header (str values size)
	/* TODO: Add isBp. */
	serializeElement(tree, res[1]); // tree
	serializeElement(names, res[2]); // name table
	serializeElement(values, res[3]); // values
	serializeElement(stringValues, res[4]); // string values

	for (uint64_t i = 0; i < nsplit; i++) {
		std::string splitname = filename + splitnames[i];
		//save_to_file(res[i], splitname);

		std::ofstream ofs(splitname, std::ios::binary);
		ofs.write((char *)&res[i][0], res[i].size());
		ofs.close();
	}
}

namespace detail {
	template <class T>
	struct deserializeHelper;

	template <class T>
	struct deserializeHelper {
		static T apply(StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			// cout << "end - begin " << end - begin << endl;
			// cout << "sizeof(T) " << sizeof(T) << endl;

			assert(begin + sizeof(T) <= end);
			T val;
			uint8_t *ptr = reinterpret_cast<uint8_t *>(&val);
			std::copy(begin, begin + sizeof(T), ptr);
			begin += sizeof(T);
			return val;
		}
	};

	void deserializeArray(char *a, int size, StreamType::const_iterator &begin,
			StreamType::const_iterator end) {
		for (uint64_t i = 0; i < size; ++i)
			a[i] = deserializeHelper<char>::apply(begin, end);
	}

	template <>
	struct deserializeHelper<std::string> {
		static std::string apply(StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			std::string str = "";
			for (char c; (c = deserializeHelper<char>::apply(begin, end));)
				str += c;
			return str;
		}
	};

	template <>
	struct deserializeHelper<char *> {
		static char *apply(StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			std::string str = deserializeHelper<std::string>::apply(begin, end);
			return string_to_cstr(str);
		}
	};

	template <>
	struct deserializeHelper<sdsl::bit_vector> {
		static sdsl::bit_vector apply(uint64_t size_in_bits, uint64_t size_in_bytes, StreamType::const_iterator &begin,
		// static bit_vector apply(int size_in_bytes, StreamType::const_iterator& begin,
				StreamType::const_iterator end) {
			// int size_in_bits = size_in_bytes * 8;
			// int size_in_bytes = bv_size_in_bytes;
			char *p = new char[size_in_bytes];

			for (uint64_t i = 0; i < size_in_bytes; ++i)
				p[i] = deserializeHelper<char>::apply(begin, end);

			sdsl::bit_vector bv = char_array_to_bitvector(p, size_in_bits);
			delete[] p;
			return bv;
		}
	};

	template <>
	struct deserializeHelper<SuccinctTree> {
		static SuccinctTree apply(uint64_t doc_size, StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			uint64_t size_in_bits = (doc_size + 1) * 2;
			uint64_t size_in_bytes = (size_in_bits + 7) / 8;

			sdsl::bit_vector bv = deserializeHelper<sdsl::bit_vector>::apply(size_in_bits, size_in_bytes, begin, end);
			return SuccinctTree(bv);
	   }
	};

	template <>
	struct deserializeHelper<PackedArray> {
		static PackedArray apply(StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			// deserialize void array size in bytes
			uint64_t size = deserializeHelper<uint64_t>::apply(begin, end);
			// allocate an array of the appropriate size
			char *array = new char[size];
			// deserialize the void array byte by byte
			deserializeArray(array, size, begin, end);
			// construct and return a PackedArray
			return PackedArray(size, array);
	   }
	};

	template <>
	struct deserializeHelper<BitmapIndex> {
		static BitmapIndex apply(uint64_t values_size, StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			// deserialize values array
			PackedArray array = deserializeHelper<PackedArray>::apply(begin, end);

			// load jval bitmap index from its values array
			BitmapIndex index(array);
			loadJvalBitmapIndex(index);
			return index;
		}
	};

	template <class T>
	struct deserializeHelper<std::vector<T> > {
		static std::vector<T> apply(uint64_t size, StreamType::const_iterator &begin,
				StreamType::const_iterator end) {
			std::vector<T> v;
			v.reserve(size);

			for (uint64_t i = 0; i < size; i++)
				v.push_back(deserializeHelper<T>::apply(begin, end));

			return v;
		}
	};
} // end detail namespace

template <class T>
T deserializeElement(StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
	return detail::deserializeHelper<T>::apply(begin, end);
}

template <class T>
T deserializeElement(uint64_t extra, StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
	return detail::deserializeHelper<T>::apply(extra, begin, end);
}

template <class T>
T deserializeElement(uint64_t extra1, uint64_t extra2, StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
	return detail::deserializeHelper<T>::apply(extra1, extra2, begin, end);
}

template <class T>
T deserializeElement(const StreamType &res) {
	StreamType::const_iterator it = res.begin();
	return deserializeElement<T>(it, res.end());
}

/*template Cjson<BpTree> deserialize(const StreamType&);
template Cjson<DfTree> deserialize(const StreamType&);
template Cjson<BpTree> deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template Cjson<DfTree> deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);*/
template sdsl::bit_vector deserializeElement(uint64_t, uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template SuccinctTree deserializeElement(uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template BitmapIndex deserializeElement(uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template std::vector<char *> deserializeElement(uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template std::vector<std::string> deserializeElement(uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template std::vector<uint64_t> deserializeElement(uint64_t, StreamType::const_iterator &, const StreamType::const_iterator &);
template char * deserializeElement(StreamType::const_iterator &, const StreamType::const_iterator &);
template std::string deserializeElement(StreamType::const_iterator &, const StreamType::const_iterator &);
template uint64_t deserializeElement(StreamType::const_iterator &, const StreamType::const_iterator &);
template double deserializeElement(StreamType::const_iterator &, const StreamType::const_iterator &);

/* Since this is a class function, we don't need to return the whole Sjson. */

Sjson Sjson::deserializeSingle(const std::string &filename) {
	std::ifstream ifs(filename, std::ios::binary);
	ifs.seekg(0, ifs.end);
	uint64_t size = ifs.tellg();
	StreamType res(size);
	ifs.seekg(0);
	ifs.read((char *)&res[0], size);
	ifs.close();

	StreamType::const_iterator begin = res.begin(), end = res.end();

	// recover header
	uint64_t psize = detail::deserializeHelper<uint64_t>::apply(begin, end);
	// recover the succinct tree
	SuccinctTree st = detail::deserializeHelper<SuccinctTree>::apply(psize, begin, end);
	// recover names table
	uint64_t namesSize = detail::deserializeHelper<uint64_t>::apply(begin, end);
	std::vector<char *> names = detail::deserializeHelper<std::vector<char *> >::apply(namesSize, begin, end); //TODO: namesSize?
	// recover values
	BitmapIndex values = detail::deserializeHelper<BitmapIndex>::apply(size, begin, end);
	// recover stringValues array
	uint64_t strvSize = detail::deserializeHelper<uint64_t>::apply(begin, end);
	//std::vector<char *> stringValues = detail::deserializeHelper<std::vector<char *> >::apply(strvSize, begin, end);
	std::string stringValues;

	//return deserialize< Cjson<T> >(res);
	return Sjson(psize, st, names, values, stringValues);
}

Sjson Sjson::deserializeMultiple(const std::string &filename) {
	const uint64_t nsplit = 5;
	std::string splitnames[] = {"_header", "_tree", "_names", "_values", "_strvalues"};

	uint64_t psize, namesSize, strvSize;
	SuccinctTree st;
	std::vector<char *> names;
	BitmapIndex values;
	std::string stringValues;
	//std::vector<char *> stringValues;

	for (uint64_t i = 0; i < nsplit; i++) {
		std::ifstream ifs(filename + splitnames[i], std::ios::binary);
		ifs.seekg(0, ifs.end);
		uint64_t size = ifs.tellg();
		StreamType res(size);
		ifs.seekg(0);
		ifs.read((char *)&res[0], size);
		ifs.close();

		StreamType::const_iterator begin = res.begin(), end = res.end();
		if (i == 0) {
			psize = deserializeElement<uint64_t>(begin, end);
			namesSize = deserializeElement<uint64_t>(begin, end);
			strvSize = deserializeElement<uint64_t>(begin, end);
		}
		else if (i == 1) st = deserializeElement<SuccinctTree>(psize, begin, end);
		else if (i == 2) names = deserializeElement<std::vector<char *> >(namesSize, begin, end);
		else if (i == 3) values = deserializeElement<BitmapIndex>(psize, begin, end);
		//else stringValues = deserializeElement<std::vector<char *> >(strvSize, begin, end);
	}

	return Sjson(psize, st, names, values, stringValues);
}

#endif

