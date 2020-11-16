#pragma once

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include "Sjson.h"
#include <cstdlib>

typedef std::vector<uint8_t> StreamType;

template <class T> size_t get_size(const T &obj);

template <class T> void serialize(const T &obj, StreamType &res);

template <class T> T deserialize(StreamType::const_iterator &, const StreamType::const_iterator &);
template <class T> T deserialize(int, StreamType::const_iterator &, const StreamType::const_iterator &);
template <class T> T deserialize(const StreamType &);

void save_to_file(StreamType &, const std::string &);

template <class T> void save_to_file(Sjson &, const std::string &);
template <class T> void save_to_file_split(Sjson &, const std::string &);
Sjson load_from_file(const std::string &);
Sjson load_from_file_split(const std::string &);

#endif

