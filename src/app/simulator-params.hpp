/**
 * simulator-params.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATOR_PARAMS_HPP
#define SIMULATOR_PARAMS_HPP

#include "glob.hpp"

#include "boundary-cond.hpp"
#include "grid.hpp"

namespace brandy0
{

struct SimulatorParams
{
	static constexpr uint32_t DEFAULT_WP = 64, MAX_WP = 16384, DEFAULT_HP = 64, MAX_HP = 16384,
					DEFAULT_STEPS_PER_FRAME = 5, MAX_STEPS_PER_FRAME = 1048576, DEFAULT_FRAME_CAPACITY = 4096, MAX_FRAME_CAPACITY = 16777216;
	static constexpr double DEFAULT_RHO = 1.0, MIN_RHO = 1e-5, MAX_RHO = 1e5,
					DEFAULT_MU = 1.0, MIN_MU = 1e-5, MAX_MU = 1e5,
					DEFAULT_W = 1.0, MIN_W = 1e-4, MAX_W = 1e4,
					DEFAULT_H = 1.0, MIN_H = 1e-4, MAX_H = 1e4,
					DEFAULT_STOP_AFTER = -1.0,
					DEFAULT_DT = .01, MIN_DT = 1e-9, MAX_DT = 1e1;

	double w, h;
	uint32_t wp, hp;
	double dt;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
	double rho, mu;
	Grid<bool> solid;
	// < 0 iff simulation should not automatically stop
	double stopAfter;
	uint32_t stepsPerFrame;
	uint32_t frameCapacity;

	// TODO add compressibility indicator as member

	SimulatorParams(const double w, const double h, const uint32_t wp, const uint32_t hp, const double dt,
			const BoundaryCond& bcx0, const BoundaryCond& bcx1, const BoundaryCond& bcy0, const BoundaryCond& bcy1,
			const double rho, const double mu, const Grid<bool>& solid, const double stopAfter, const uint32_t stepsPerFrame,
			const uint32_t frameCapacity)
		: w(w), h(h), wp(wp), hp(hp), dt(dt), bcx0(bcx0), bcx1(bcx1), bcy0(bcy0), bcy1(bcy1), rho(rho), mu(mu), solid(solid),
		stopAfter(stopAfter), stepsPerFrame(stepsPerFrame), frameCapacity(frameCapacity)
	{
	}

};

}

#endif // SIMULATOR_PARAMS_HPP

