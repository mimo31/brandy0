/**
 * shape-config-window-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/17
 */
#ifndef SHAPE_CONFIG_WINDOW_ABSTR_HPP
#define SHAPE_CONFIG_WINDOW_ABSTR_HPP

#include "listener-manager.hpp"
#include "vec.hpp"
#include "vec2d.hpp"

namespace brandy0
{

class ShapeConfigWindowAbstr
{
public:
	vec<vec2d> nextShapeClicks;
	uint32_t addShapeMode;

	ListenerManager nextShapeChangeListeners;

};

}

#endif // SHAPE_CONFIG_WINDOW_ABSTR_HPP