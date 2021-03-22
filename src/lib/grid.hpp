/**
 * grid.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef GRID_HPP
#define GRID_HPP

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>

#include "point.hpp"

namespace brandy0
{

template <typename T>
struct Grid
{
	T *data;
	uint32_t w, h;

	Grid(const uint32_t w, const uint32_t h) : w(w), h(h)
	{
		if (w == 0 || h == 0)
			data = nullptr;
		else
			data = new T[w * h];
	}

	Grid(const Grid& g) : Grid(g.w, g.h)
	{
		if (w != 0 && h != 0)
			std::copy_n(g.data, w * h, data);
	}

	~Grid()
	{
		if (w != 0 && h != 0)
			delete[] data;
	}

	T &operator()(const uint32_t x, const uint32_t y) const
	{
		assert(x >= 0);
		assert(x < w);
		assert(y >= 0);
		assert(y < h);
		return data[x + y * w];
	}

	T &operator()(const Point& p) const
	{
		return operator()(p.x, p.y);
	}

	Grid &operator=(const Grid &other)
	{
		if (w * h != other.w * other.h)
		{
			if (w != 0 && h != 0)
				delete[] data;
			w = other.w;
			h = other.h;
			if (w != 0 && h != 0)
				data = new T[w * h];
		}
		else if (w != other.w || h != other.h)
		{
			w = other.w;
			h = other.h;
		}
		if (w != 0 && h != 0)
			std::copy_n(other.data, w * h, data);
		return *this;
	}

	void set_all(const T val)
	{
		for (uint32_t y = 0; y < h; y++)
		{
			for (uint32_t x = 0; x < w; x++)
			{
				data[x + y * w] = val;
			}
		}
	}
};

template <typename T>
T max(const Grid<T>& grid)
{
	T best = grid.data[0];
	for (uint32_t i = 1; i < grid.w * grid.h; i++)
		best = std::max(best, grid.data[i]);
	return best;
}

template <typename T>
T min(const Grid<T>& grid)
{
	T best = grid.data[0];
	for (uint32_t i = 1; i < grid.w * grid.h; i++)
		best = std::min(best, grid.data[i]);
	return best;
}

template <typename T, typename S>
S max(const Grid<T>& grid, const std::function<S(T)>& evaluator)
{
	S best = evaluator(grid.data[0]);
	for (uint32_t i = 1; i < grid.w * grid.h; i++)
		best = std::max(best, evaluator(grid.data[i]));
	return best;
}

}

#endif // GRID_HPP
