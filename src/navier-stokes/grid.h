/**
 * grid.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef GRID_H
#define GRID_H

#include <global.h>

namespace brandy0
{

template <typename T>
struct Grid
{
    T *data;

    Grid()
    {
        data = new T[n * n];
    }

    ~Grid()
    {
        delete[] data;
    }

    T &operator()(const uint32_t x, const uint32_t y) const
    {
        return data[x + y * n];
    }

    Grid &operator=(const Grid &other)
    {
        copy_n(other.data, n * n, data);
        return *this;
    }
};

}

#endif // GRID_H