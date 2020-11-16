#!/bin/bash
set -e
g++ -O3 BitmapIndex.cpp Jval.cpp Main.cpp Ostream.cpp ParseJson.cpp Query.cpp Serialize.cpp Sjson.cpp Util.cpp -ldivsufsort -ldivsufsort64 -lsdsl -o sjson
g++ -DCSA -O3 BitmapIndex.cpp Jval.cpp Main.cpp Ostream.cpp ParseJson.cpp Query.cpp Serialize.cpp Sjson.cpp Util.cpp -ldivsufsort -ldivsufsort64 -lsdsl -o sjson-csa
