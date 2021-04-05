/**
 * shape-config-window-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/17
 */
#ifndef SHAPE_CONFIG_WINDOW_ABSTR_HPP
#define SHAPE_CONFIG_WINDOW_ABSTR_HPP

#include "listener-manager.hpp"
#include "vec.hpp"
#include "vec2d.hpp"

namespace brandy0
{

/**
 * The abstract interface for the shape configuration window for its child shape configuration widget
 * (@see ApplicationAbstr for more explanation on the *Abstr classes)
 */
class ShapeConfigWindowAbstr
{
public:
	/**
	 * List of points that were clicked as part of the addition of the current shape
	 * (e.g. vertices when a polygon is being added or the center when a circle is being added)
	 */
	vec<vec2d> nextShapeClicks;
	/// The current shape adding mode
	uint32_t addShapeMode;

	/// Listeners for the event when the users somehow changes the shape that is being added (e.g. completes the shape or addes a new vertex)
	ListenerManager nextShapeChangeListeners;

};

}

#endif // SHAPE_CONFIG_WINDOW_ABSTR_HPP