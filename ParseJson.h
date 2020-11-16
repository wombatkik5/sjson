#pragma once

#ifndef __PARSEJSON_H__
#define __PARSEJSON_H__

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include "BitmapIndex.h"
#include "half.hpp"
#include "Jval.h"
#include "Sjson.h"

bool parseHasName; //Global variable checking whether an element has a name.
uint64_t parseNameId; //Global variable storing id of a name.

#endif //__PARSEJSON_H__
