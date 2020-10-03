/**
 * state.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef STATE_H
#define STATE_H

#include "vec2d.h"
#include "grid.h"

namespace brandy0
{

struct State
{
    Grid<vec2d> u;
    Grid<double> p;
};

}

#endif // STATE_H