/**
 * linearvar.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef LINEARVAR_H
#define LINEARVAR_H

#include <stdint.h>

namespace brandy0
{

constexpr uint32_t CONST_VAR = UINT32_MAX;

/**
 * A struct representing a variable (identified by an integer) with a constant coefficient (double)
 */
struct LinearVar
{
    double mult;
    uint32_t var;

    LinearVar(const double mult, const uint32_t var) : mult(mult), var(var)
    {
    }
};

}

#endif // LINEARVAR_H