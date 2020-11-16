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
	auto serializeBeginTime = std::chrono::system_clock::now();
	p->serializeSingle("test.ser");
	auto serializeEndTime = std::chrono::system_clock::now();
	auto serializeTime = std::chrono::duration_cast<std::chrono::microseconds>(
	serializeEndTime - serializeBeginTime);
	std::cout << "Serialization complete." << std::endl;
	std::cout << "Serialization took " << serializeTime.count() << " microseconds." << std::endl;

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
				case 'v': /* retrieve value. */
				{
					std::cout << "Enter id: ";
					std::cin >> uintTwo;
					auto valueQueryBeginTime = std::chrono::system_clock::now();
					p->queryArrValues(nodeId, uintTwo);
					auto valueQueryEndTime = std::chrono::system_clock::now();
					auto valueQueryTime = std::chrono::duration_cast<std::chrono::microseconds>(valueQueryEndTime - valueQueryBeginTime);
					std::cout << "Query took " << valueQueryTime.count() << " microseconds." << std::endl;
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

#endif
