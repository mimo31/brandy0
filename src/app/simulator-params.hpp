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
	double w, h;
	uint32_t wp, hp;
	double dt;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
	double rho, mu;
	Grid<bool> solid;

	SimulatorParams(const double w, const double h, const uint32_t wp, const uint32_t hp, const double dt,
			const BoundaryCond& bcx0, const BoundaryCond& bcx1, const BoundaryCond& bcy0, const BoundaryCond& bcy1,
			const double rho, const double mu, const Grid<bool>& solid)
		: w(w), h(h), wp(wp), hp(hp), dt(dt), bcx0(bcx0), bcx1(bcx1), bcy0(bcy0), bcy1(bcy1), rho(rho), mu(mu), solid(solid)
	{
	}

};

}

#endif // SIMULATOR_PARAMS_HPP

