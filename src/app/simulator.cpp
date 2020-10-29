/**
 * simulator.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#include "simulator.hpp"

#include <iostream>

namespace brandy0
{

using std::cout;
using std::endl;

Simulator::Simulator(const SimulatorParams& params)
	: bcx0(params.bcx0), bcx1(params.bcx1), bcy0(params.bcy0), bcy1(params.bcy1),
	solid(params.solid), indep(params.w, params.h),
	u0(params.w, params.h), u1(params.w, params.h),
	p0(params.w, params.h), p1(params.w, params.h)
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
	/*bcx0 = params.bcx0;
	bcx1 = params.bcx1;
	bcy0 = params.bcy0;
	bcy1 = params.bcy1;*/

	//solid = params.solid;

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

	//u0 = u1 = Grid<vec2d>(w, h);
	//p0 = p1 = Grid<double>(w, h);
}

void Simulator::iter()
{
	cout << "iter" << endl;
}

}
