#pragma once



#include "BufferedReader.hpp"
#include "BufferedWriter.hpp"
#include "Concepts.hpp"



namespace Strawberry::Core::IO
{
	template <typename T, size_t S = 1024> requires Read<T> && Write<T>
	using BufferedIO = BufferedReader<BufferedWriter<T>, S>;
}