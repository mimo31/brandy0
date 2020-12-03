/**
 * boundary-cond.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef BOUNDARY_COND_HPP
#define BOUNDARY_COND_HPP

#include "glob.hpp"

#include "vec2d.hpp"

namespace brandy0
{

enum PressureBoundaryCond
{
	DIRICHLET, VON_NEUMANN
};

struct BoundaryCond
{
	PressureBoundaryCond p;
	vec2d u;

	BoundaryCond(const vec2d u, const PressureBoundaryCond p) : u(u), p(p)
	{
	}
};

}

#endif // BOUNDARY_COND_HPP
