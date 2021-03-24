/**
 * sim-frame.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIM_FRAME_HPP
#define SIM_FRAME_HPP

#include "grid.hpp"
#include "vec2d.hpp"

namespace brandy0
{

class SimFrame
{
public:
	Grid<double> p;
	Grid<vec2d> u;

	SimFrame(const Grid<double>& p, const Grid<vec2d>& u)
		: p(p), u(u)
	{
	}
};

}

#endif // SIM_FRAME_HPP
