/**
 * simulator.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "grid.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class Simulator
{
public:
	double w, h;
	Simulator(const SimulatorParams& params);
	void iter();
	Grid<double> p0, p1;
	Grid<vec2d> u0, u1;

private:
	double dt;
	double dx, dy;
	uint32_t wp, hp;
	double rho, mu, nu;
	Grid<bool> solid;
	Grid<bool> indep;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
};

}

#endif // SIMULATOR_HPP
