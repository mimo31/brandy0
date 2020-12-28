/**
 * grid.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef GRID_HPP
#define GRID_HPP

#include <algorithm>
#include <cstdint>

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
		data = new T[w * h];
	}

	Grid(const Grid& g) : Grid(g.w, g.h)
	{
		std::copy_n(g.data, w * h, data);
	}

	~Grid()
	{
		delete[] data;
	}

	T &operator()(const uint32_t x, const uint32_t y) const
	{
		return data[x + y * w];
	}

	T &operator()(const Point& p) const
	{
		return data[p.x + p.y * w];
	}

	Grid &operator=(const Grid &other)
	{
		if (w * h != other.w * other.h)
		{
			delete[] data;
			w = other.w;
			h = other.h;
			data = new T[w * h];
		}
		else if (w != other.w || h != other.h)
		{
			w = other.w;
			h = other.h;
		}
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

}

#endif // GRID_HPP
