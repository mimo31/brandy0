/**
 * simulator.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#include "simulator.hpp"

namespace brandy0
{

Simulator::Simulator(const SimulationParams& params)
	: w(params.w), h(params.h), crashed(false), incomplete(false),
	f0(Grid<double>(params.wp, params.hp), Grid<vec2d>(params.wp, params.hp)),
	f1(Grid<double>(params.wp, params.hp), Grid<vec2d>(params.wp, params.hp)),
	dt(params.dt),
	dx(w / (params.wp - 1)), dy(h / (params.hp - 1)),
	wp(params.wp), hp(params.hp),
	rho(params.rho), mu(params.mu), nu(params.mu / params.rho),
	solid(params.wp, params.hp), indep(params.wp, params.hp),
	bcx0(params.bcx0), bcx1(params.bcx1), bcy0(params.bcy0), bcy1(params.bcy1)
{
	params.shapeStack.set(solid);
	for (uint32_t x = 0; x < wp; x++)
	{
		indep(x, 0) = indep(x, hp - 1) = false;
	}
	for (uint32_t y = 0; y < hp; y++)
	{
		indep(0, y) = indep(wp - 1, y) = false;
	}
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			indep(x, y) = !(solid(x, y) || solid(x + 1, y)
				|| solid(x - 1, y) || solid(x, y + 1) || solid(x, y - 1));
		}
	}
	f0.p.set_all(0);
	f1.p.set_all(0);
	f0.u.set_all(vec2d(0, 0));
	f1.u.set_all(vec2d(0, 0));
}

void Simulator::setPauseControl(const bool *const pauseSignal, std::mutex *const controlMutex)
{
	this->pauseSignal = pauseSignal;
	this->controlMutex = controlMutex;
}

}
