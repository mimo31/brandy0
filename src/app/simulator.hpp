/**
 * simulator.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "glob.hpp"

#include "grid.hpp"
#include "sim-frame.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class Simulator
{
public:
	double w, h;
	Simulator(const SimulatorParams& params);
	virtual void iter() = 0;
	SimFrame f0, f1;

protected:
	double dt;
	double dx, dy;
	uint32_t wp, hp;
	double rho, mu, nu;
	Grid<bool> solid;
	Grid<bool> indep;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
	vec2d to_coor(const Point&);
	vec2d to_coor(const int32_t x, const int32_t y);
};

}

#endif // SIMULATOR_HPP
