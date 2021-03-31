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

struct SimulationParams
{

	double w, h;
	uint32_t wp, hp;
	double dt;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
	double rho, mu;
	// Grid<bool> solid;
	ObstacleShapeStack shapeStack;
	// < 0 iff simulation should not automatically stop
	double stopAfter;
	uint32_t stepsPerFrame;
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

	double get_dx() const
	{
		return w / (wp - 1);
	}

	double get_dy() const
	{
		return h / (hp - 1);
	}

};

}

#endif // SIMULATION_PARAMS_HPP

