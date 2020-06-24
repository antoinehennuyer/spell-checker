cmake_minimum_required(VERSION 3.0)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++14 -O3")

project(textmining)

add_executable(
	TextMiningCompiler
	src/TextMiningCompiler.cpp
	src/PatriciaTrie.hh
	)
add_executable(
	TextMiningApp
	src/TextMiningApp.cpp
	src/PatriciaTrie.hh
	)
