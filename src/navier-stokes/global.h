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

constexpr uint32_t n = 64;

constexpr double penv = 1;
constexpr vec2d uenv(0, -.001);

constexpr double midr = .1143;

}

#endif // GLOBAL_H