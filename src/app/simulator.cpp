/**
 * simulator.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#include "simulator.hpp"

namespace brandy0
{

Simulator::Simulator(const SimulatorParams& params)
	: bcx0(params.bcx0), bcx1(params.bcx1), bcy0(params.bcy0), bcy1(params.bcy1),
	solid(params.solid), indep(params.wp, params.hp),
	f0(Grid<double>(params.wp, params.hp), Grid<vec2d>(params.wp, params.hp)),
	f1(Grid<double>(params.wp, params.hp), Grid<vec2d>(params.wp, params.hp))
{
	w = params.w;
	h = params.h;
	wp = params.wp;
	hp = params.hp;
	dx = w / (wp - 1);
	dy = h / (hp - 1);
	dt = params.dt;
	rho = params.rho;
	mu = params.mu;
	nu = rho / mu;

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

vec2d Simulator::to_coor(const Point& p)
{
	return vec2d(p.x * w / (wp - 1), p.y * h / (hp - 1));
}

vec2d Simulator::to_coor(const int32_t x, const int32_t y)
{
	return to_coor(Point(x, y));
}

}
