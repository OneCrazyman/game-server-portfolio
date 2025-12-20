#pragma once
#include <vector>
#include <numeric>
#include <random>
class DataSetMaker
{
public:
	std::vector<int> GetSortedVector(int size)
	{
		std::vector<int> v(size);
		std::iota(v.begin(), v.end(), 1);
		return v;
	}

	std::vector<int> GetRandomVector(int size, int seed)
	{
		auto v = GetSortedVector(size);

		std::mt19937 mt(seed);
		std::shuffle(v.begin(), v.end(), mt);
		return v;
	}
};