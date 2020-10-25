/**
 * global.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <iostream>

#include "vec2d.h"

using std::cout;
using std::endl;

namespace brandy0
{

constexpr uint32_t n = 129;
constexpr double dx = 2 / double(n - 1);
constexpr double dt = .00002;

constexpr double u0 = 48;
constexpr double nu = .1;

}

#endif // GLOBAL_H