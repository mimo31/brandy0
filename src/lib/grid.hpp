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

/**
 * Templated struct for holding a two-dimensional array of data.
 * T is the type of which the two-dimensional array should consist (its entries).
 */
template <typename T>
struct Grid
{
	/**
	 * Pointer to the data (the two-dimensional array stored in row major order).
	 * Should be nullptr iff either w or h is 0.
	 */
	T *data;
	/// Width of the two-dimensional array
	uint32_t w;
	/// Height of the two-dimensional array
	uint32_t h;

	/**
	 * Creates a Grid object of specified dimenions
	 * @param w width of the array
	 * @param h height of the array
	 */
	Grid(const uint32_t w, const uint32_t h) : w(w), h(h)
	{
		if (w == 0 || h == 0)
			data = nullptr;
		else
			data = new T[w * h];
	}

	Grid(const Grid &g) : Grid(g.w, g.h)
	{
		if (w != 0 && h != 0)
			std::copy_n(g.data, w * h, data);
	}

	~Grid()
	{
		if (w != 0 && h != 0)
			delete[] data;
	}

	/**
	 * @param x x index of an entry
	 * @param y y index of an entry
	 * @return reference to the entry in the two-dimensional array
	 */
	T &operator()(const uint32_t x, const uint32_t y) const
	{
		assert(x < w);
		assert(y < h);
		return data[x + y * w];
	}

	/**
	 * @param p (x and y) indices of an entry
	 * @return reference to the entry in the two-dimensional array
	 */
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

	/**
	 * Sets all entries in the array to a specified value
	 * @param val value to set all entries to
	 */
	void set_all(const T val)
	{
		std::fill_n(data, w * h, val);
	}
};

/**
 * Finds the maximum value in a grid. The type T should overload the < operator.
 * @param grid grid to find the maximum in (should contain at least one entry)
 * @return maximum value of an entry in the gird
 */
template <typename T>
T max(const Grid<T> &grid)
{
	assert(grid.data != nullptr);
	return *std::max_element(grid.data, grid.data + grid.w * grid.h);
}

/**
 * Finds the minimum value in a grid. The type T should overload the < operator.
 * @param grid grid to find the minimum in (should contain at least one entry)
 * @return minimum value of an entry in the gird
 */
template <typename T>
T min(const Grid<T> &grid)
{
	assert(grid.data != nullptr);
	return *std::min_element(grid.data, grid.data + grid.w * grid.h);
}

/**
 * Finds the minimum value of a function applied on an entry in a grid. The type S should overload the < operator.
 * @param grid grid to take the entries from
 * @param evaluator function that assigns values to entries in the grid
 * @return maximum of all the values that the specified function produces for entries in the grid
 */
template <typename T, typename S>
S max(const Grid<T> &grid, const std::function<S(T)> &evaluator)
{
	S best = evaluator(grid.data[0]);
	for (uint32_t i = 1; i < grid.w * grid.h; i++)
		best = std::max(best, evaluator(grid.data[i]));
	return best;
}

}

#endif // GRID_HPP
