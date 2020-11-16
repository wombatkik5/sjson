#ifndef __MAIN_CPP__
#define __MAIN_CPP__

#include <algorithm>
#include <chrono>
#include <iostream>
#include "Sjson.h"

bool CheckArguments(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cout << "<Usage>" << std::endl;
		std::cout << argv[0] << " <JSON file>" << std::endl;
		return false;
	}
	return true;
}


/* TODO: Summarize options that I would like to apply and make an additional function. */
int main(int argc, char *argv[])
{
	if(!CheckArguments(argc, argv))
	{
		return 1;
	}

	Sjson *p;
	auto parseBeginTime = std::chrono::system_clock::now();
	#ifdef CSA
	p = new Sjson(argv[1], true);
	#else
	p = new Sjson(argv[1], false);
	#endif
	auto parseEndTime = std::chrono::system_clock::now();
	auto parseTime = std::chrono::duration_cast<std::chrono::microseconds>(parseEndTime - parseBeginTime);
	std::cout << "Parsing took " << parseTime.count() << " microseconds." << std::endl;
	std::cout << "Number of elements is " << p->tree.size() / 2 << "." << std::endl;
	//std::cout << sdsl::size_in_bytes(p->tree._bv) << std::endl;
	//std::cout << sdsl::size_in_bytes(*p->tree.bps) << std::endl;

	/*
	auto textBeginTime = std::chrono::system_clock::now();
	std::cout << p->stringValues[500] << std::endl;
	auto textEndTime = std::chrono::system_clock::now();
	auto textTime = std::chrono::duration_cast<std::chrono::microseconds>(textEndTime - textBeginTime);
	std::cout << "Took " << textTime.count() << " microseconds." << std::endl;
	textBeginTime = std::chrono::system_clock::now();
        std::cout << p->stringValues[2500] << std::endl;
        textEndTime = std::chrono::system_clock::now();
        textTime = std::chrono::duration_cast<std::chrono::microseconds>(textEndTime - textBeginTime);
        std::cout << "Took " << textTime.count() << " microseconds." << std::endl;
	textBeginTime = std::chrono::system_clock::now();
        std::cout << p->stringArray.Get(0, 10) << std::endl;
        textEndTime = std::chrono::system_clock::now();
        textTime = std::chrono::duration_cast<std::chrono::microseconds>(textEndTime - textBeginTime);
        std::cout << "Took " << textTime.count() << " microseconds." << std::endl;
        textBeginTime = std::chrono::system_clock::now();
        std::cout << p->stringArray.Get(2000, 2020) << std::endl;
        textEndTime = std::chrono::system_clock::now();
        textTime = std::chrono::duration_cast<std::chrono::microseconds>(textEndTime - textBeginTime);
        std::cout << "Took " << textTime.count() << " microseconds." << std::endl;
        textBeginTime = std::chrono::system_clock::now();
        std::cout << p->stringArray.Get(10000, 10050) << std::endl;
        textEndTime = std::chrono::system_clock::now();
        textTime = std::chrono::duration_cast<std::chrono::microseconds>(textEndTime - textBeginTime);
        std::cout << "Took " << textTime.count() << " microseconds." << std::endl;
	*/

	/* Serialize the result. */
	//std::cout << "Do you want to serialize the result? ";
	char se;
	//std::cin >> se;
	//if(se == 'y')
	//{
	//	std::cout << "Which type of serialization? ";
		se = 's';
		//std::cin >> se;
		if(se == 's')
		{
			auto serializeBeginTime = std::chrono::system_clock::now();
			p->serializeSingle("test.ser");
			auto serializeEndTime = std::chrono::system_clock::now();
			auto serializeTime = std::chrono::duration_cast<std::chrono::microseconds>(
			serializeEndTime - serializeBeginTime);
			std::cout << "Serialization complete." << std::endl;
			std::cout << "Serialization took " << serializeTime.count() << " microseconds." << std::endl;
		}
		else if(se == 'm')
		{
			auto serializeBeginTime = std::chrono::system_clock::now();
			p->serializeMultiple("test.ser");
			auto serializeEndTime = std::chrono::system_clock::now();
			auto serializeTime = std::chrono::duration_cast<std::chrono::microseconds>(
			serializeEndTime - serializeBeginTime);
			std::cout << "Serialization complete." << std::endl;
			std::cout << "Serialization took " << serializeTime.count() << " microseconds." << std::endl;
		}
	//}
	//return 0;

	uint64_t nodeId = 0;

	while(true)
	{
		char op;
		std::string str, strTwo;
		uint8_t nodeType;
		uint64_t uint, uintTwo;
		std::cout << "Current node ID is " << nodeId << "." << std::endl;
		std::cout << "Its type is ";
		nodeType = p->queryType(nodeId);
		switch(nodeType)
		{
			case 1:
				std::cout << "null." << std::endl;
				break;
			case 2:
				std::cout << "object." << std::endl;
				break;
			case 3:
				std::cout << "array." << std::endl;
				break;
			case 4:
				std::cout << "false." << std::endl;
				break;
			case 5:
				std::cout << "true." << std::endl;
				break;
			case 6:
				std::cout << "string." << std::endl;
				break;
			case 7:
				std::cout << "number." << std::endl;
				break;
			default:
				std::cout << "error." << std::endl;
		}
		if(nodeType == 2) /* Object. */
		{
			std::cout << "Enter query to execute: ";
			if(!(std::cin >> op))
			{
				std::cin.clear();
				while(std::cin.get() != '\n');
				continue;
			}
			switch(op)
			{
				case 'c': /* go to child. */
				{
					std::cout << "Enter i: ";
					std::cin >> uintTwo;
					auto childQueryBeginTime = std::chrono::system_clock::now();
					nodeId = p->queryChild(nodeId, uintTwo);
					auto childQueryEndTime = std::chrono::system_clock::now();
					auto childQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(childQueryEndTime - childQueryBeginTime);
					std::cout << "Query took " << childQueryTime.count() << " microseconds." << std::endl;
				}
					break;
				case 'n': /* list names. */
				{
					auto nameQueryBeginTime = std::chrono::system_clock::now();
					p->queryNames(nodeId);
					auto nameQueryEndTime = std::chrono::system_clock::now();
                                        auto nameQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(nameQueryEndTime - nameQueryBeginTime);
					std::cout << "Query took " << nameQueryTime.count() << " microseconds." << std::endl;
				}
					break;
				case 'p': /* go to parent. */
				{
					auto parentQueryBeginTime = std::chrono::system_clock::now();
					nodeId = p->queryParent(nodeId);
					auto parentQueryEndTime = std::chrono::system_clock::now();
                                        auto parentQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(parentQueryEndTime - parentQueryBeginTime);
                                        std::cout << "Query took " << parentQueryTime.count() << " microseconds." << std::endl;
				}
					break;
				case 'v': /* retrieve value. */
				{
					std::cout << "Enter name: ";
					std::cin >> str;
					auto valueQueryBeginTime = std::chrono::system_clock::now();
					p->queryObjValues(nodeId, str);
					auto valueQueryEndTime = std::chrono::system_clock::now();
                                        auto valueQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(valueQueryEndTime - valueQueryBeginTime);
                                        std::cout << "Query took " << valueQueryTime.count() << " microseconds." << std::endl;
				}
					break;
				case '\n':
					std::cout << "Weird." << std::endl;
					break;
				default:
					delete p;
					return 0;
					//std::cout << "Invalid argument." << std::endl;
			}
		}
		else if(nodeType == 3) /* Array. */
		{
			std::cout << "Enter query to execute: ";
			if(!(std::cin >> op))
			{
				std::cin.clear();
				while(std::cin.get() != '\n');
				continue;
			}
			switch(op)
			{
				case 'c': /* go to child. */
					std::cout << "Enter i: ";
					std::cin >> uintTwo;
					nodeId = p->queryChild(nodeId, uintTwo);
					break;
				case 'p': /* go to parent. */
					nodeId = p->queryParent(nodeId);
					break;
				case 's': /* size of array. */
				{
					auto sizeQueryBeginTime = std::chrono::system_clock::now();
					std::cout << "Size is " << p->queryDegree(nodeId) << "." << std::endl;
					auto sizeQueryEndTime = std::chrono::system_clock::now();
                                        auto sizeQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(sizeQueryEndTime - sizeQueryBeginTime);
                                        std::cout << "Query took " << sizeQueryTime.count() << " microseconds." << std::endl;
				}
					break;
				default:
					delete p;
					return 0;
					//std::cout << "Invalid argument." << std::endl;
			}
		}
		else /* Others. */
		{
			/* Get name. */
			std::cout << "Name is " << p->queryName(nodeId) << "." << std::endl;

			/* Get value. */
			auto valueQueryBeginTime = std::chrono::system_clock::now();
			std::cout << "Value is ";
			p->queryValue(nodeId);
			auto valueQueryEndTime = std::chrono::system_clock::now();
			std::cout << "." << std::endl;
			auto valueQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(valueQueryEndTime - valueQueryBeginTime);
			std::cout << "Query took " << valueQueryTime.count() << " microseconds." << std::endl;

			/* Go to parent. */
			std::cout << "Getting back to the parent." << std::endl;
			nodeId = p->queryParent(nodeId);
		}
	}
}

/*
void write_formatted(ofstream &f, const string &msg, long &a, size_t &b) {
	f << msg << "\t\t(" << (int)((double)a/b*100) << "%)\t\t" << a << endl;
}


template <class T>
void log_cjson_size(Cjson<T> &obj, const char *fn, long orig) {
	ofstream mfile(fn, ios::out);
	// original file size
	mfile << "original size (bytes): " << orig << endl;

	long header, tree, namel, names, values;
	// size of header
	size_t s = sizeof(int);
	header = s;
	// size of tree
	tree = get_size(obj.tree);
	s += tree;
	namel = get_size(obj.nameList);
	s += namel;
	// size of names
	names = get_size(obj.names);
	s += names;
	// size of values
	values = get_size(obj.values);
	s += values;

	write_formatted(mfile, ".header", header, s);
	write_formatted(mfile, ".tree", tree, s);
	write_formatted(mfile, ".names", names, s);
	write_formatted(mfile, ".namel", namel, s);
	write_formatted(mfile, ".values", values, s);
	mfile << "Total " << s << endl;

	// compression ratio
	mfile << "Compression ratio (bigger is better): " << (double)orig/s << endl;
	mfile << endl;

	cout << "original size: " << orig << endl;
	cout << "compressed: " << s << endl;
	cout << "ratio (bigger better): " << (double)orig/s << endl << endl;

	mfile.close();
}
 */

#endif
