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

/**
 * Represents the (mathematical) type of a boundary condition. 
 * Specifically, whether it's a Dirichlet or a Neumann type boundary condition
 */
enum BoundaryCondType
{
	Dirichlet, Neumann
};

struct BoundaryCond
{
	/// Type of the boundary condition for velocity
	BoundaryCondType utype;
	/// Only relevant iff utype is Dirichlet. Value of the velocity vector at the boundary
	vec2d u;

	/// Type of the boundary condition for pressure
	BoundaryCondType ptype;
	/// Only relevant iff ptype is Dirichlet. Value of pressure at the boundary
	double p;

	BoundaryCond() = default;

	BoundaryCond(const BoundaryCondType utype, const vec2d u, const BoundaryCondType ptype, const double p)
		: utype(utype), u(u), ptype(ptype), p(p)
	{
	}
};

}

#endif // BOUNDARY_COND_HPP
