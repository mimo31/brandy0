/**
 * statgen.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef STATGEN_H
#define STATGEN_H

#include <string>

#include "vec2d.h"
#include "simulator.h"

namespace brandy0
{

void print_stats(const Simulator& s, const uint32_t iteration);

}

#endif // STATGEN_H