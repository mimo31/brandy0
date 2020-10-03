/**
 * simulator.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "grid.h"
#include "vec2d.h"
#include "state.h"
#include "linearvar.h"
#include "vartype.h"

namespace brandy0
{

class Simulator
{

private:

    Grid<bool> safepoints;
    Grid<uint32_t> pointnums;
    uint32_t freecount = 0;
    std::vector<LinearVar>* vardependence;
    
    void init_values();
    void init_free_safe_indicators();
    void init_var_dependencies();

    std::vector<LinearVar>& vdep(const int32_t x, const int32_t y, const VarType vtype);

public:

    void init()
    {
        init_values();
        init_free_safe_indicators();
        init_var_dependencies();
    }
    void iter();
    State s0, s1;
    Grid<bool> freepoints;

};

}

#endif // SIMULATOR_H