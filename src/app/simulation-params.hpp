/**
 * simulation-params.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATION_PARAMS_HPP
#define SIMULATION_PARAMS_HPP

#include "boundary-cond.hpp"
#include "grid.hpp"
#include "obstacle-shape.hpp"

namespace brandy0
{

/**
 * Struct containing all parameters of a simulation
 */
struct SimulationParams
{
	/// Physical width of the container
	double w;
	/// Physical height of the container
	double h;
	/// Number of grid points along the x axis (width of the grid)
	uint32_t wp;
	/// Number of grid points along the y axis (height of the grid)
	uint32_t hp;
	/// Time step
	double dt;
	/// Left boundary condition (at x = 0)
	BoundaryCond bcx0;
	/// Right boundary condition (at x = w)
	BoundaryCond bcx1;
	/// Bottom boundary condition (at y = 0)
	BoundaryCond bcy0;
	/// Top boundary condition (at y = h)
	BoundaryCond bcy1;
	/// Fluid density
	double rho;
	/// Fluid (dynamic) viscosity
	double mu;
	/// Stack of obstacle shapes
	ObstacleShapeStack shapeStack;
	/**
	 * Simulation time at which the computation automatically pauses (to prevent generating too many unwanted frames).
	 * Iff < 0, the computation does not automatically stop.
	 * not implemented yet
	 * TODO: implement (setting in configuration and the expected behavior when >= 0 (computation pausing)
	 */
	double stopAfter;
	/// Number of steps in each frame
	uint32_t stepsPerFrame;
	/// Capacity for computed frames (maximum number of computed frames stored at once)
	uint32_t frameCapacity;

	// TODO add compressibility indicator as member

	SimulationParams(const double w, const double h, const uint32_t wp, const uint32_t hp, const double dt,
			const BoundaryCond& bcx0, const BoundaryCond& bcx1, const BoundaryCond& bcy0, const BoundaryCond& bcy1,
			const double rho, const double mu, const ObstacleShapeStack& shapeStack, const double stopAfter, const uint32_t stepsPerFrame,
			const uint32_t frameCapacity)
		: w(w), h(h), wp(wp), hp(hp), dt(dt), bcx0(bcx0), bcx1(bcx1), bcy0(bcy0), bcy1(bcy1), rho(rho), mu(mu), shapeStack(shapeStack),
		stopAfter(stopAfter), stepsPerFrame(stepsPerFrame), frameCapacity(frameCapacity)
	{
	}

	/**
	 * @return spacial step of the discretization grid in the x direction
	 */
	double get_dx() const
	{
		return w / (wp - 1);
	}

	/**
	 * @return spacial step of the discretization grid in the y direction
	 */
	double get_dy() const
	{
		return h / (hp - 1);
	}

};

}

#endif // SIMULATION_PARAMS_HPP

