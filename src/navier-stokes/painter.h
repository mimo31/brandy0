/**
 * painter.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef PAINTER_H
#define PAINTER_H

#include <string>

#include "vec2d.h"
#include "state.h"

namespace brandy0
{

void save(const State& s, const std::string& filename);

}

#endif // PAINTER_H