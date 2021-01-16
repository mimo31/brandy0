/**
 * simulator-classic.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/26
 */
#include "simulator-classic.hpp"

namespace brandy0
{

SimulatorClassic::SimulatorClassic(const SimulatorParams& params)
	: Simulator(params), field(params.wp, params.hp), lapL1limit(.1)
{   
}

void SimulatorClassic::enforcePBoundary(SimFrame& f)
{
	if (bcx0.p == PressureBoundaryCond::DIRICHLET)
	{
		for (uint32_t y = 0; y < hp; y++)
			f.p(0, y) = 0;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			f.p(0, y) = f.p(1, y);
	}
	if (bcx1.p == PressureBoundaryCond::DIRICHLET)
	{
		for (uint32_t y = 0; y < hp; y++)
			f.p(wp - 1, y) = 0;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			f.p(wp - 1, y) = f.p(wp - 2, y);
	}
	if (bcy0.p == PressureBoundaryCond::DIRICHLET)
	{
		for (uint32_t x = 0; x < wp; x++)
			f.p(x, 0) = 0;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			f.p(x, 0) = f.p(x, 1);
	}
	if (bcy1.p == PressureBoundaryCond::DIRICHLET)
	{
		for (uint32_t x = 0; x < wp; x++)
			f.p(x, hp - 1) = 0;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			f.p(x, hp - 1) = f.p(x, hp - 2);
	}
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (solid(x, y) || indep(x, y))
				continue;
			uint32_t cou = 0;
			double sm = 0;
			if (indep(x - 1, y))
			{
				sm += f.p(x - 1, y);
				cou++;
			}
			if (indep(x + 1, y))
			{
				sm += f.p(x + 1, y);
				cou++;
			}
			if (indep(x, y - 1))
			{
				sm += f.p(x, y - 1);
				cou++;
			}
			if (indep(x, y + 1))
			{
				sm += f.p(x, y + 1);
				cou++;
			}
			f.p(x, y) = cou != 0 ? sm / cou : 0;
		}
	}
}

void SimulatorClassic::enforceUBoundary(SimFrame& f)
{
	for (uint32_t y = 0; y < hp; y++)
	{
		f.u(0, y) = bcx0.u;
		f.u(wp - 1, y) = bcx1.u;
	}
	for (uint32_t x = 0; x < wp; x++)
	{
		f.u(x, 0) = bcy0.u;
		f.u(x, hp - 1) = bcy1.u;
	}
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (!solid(x, y) && !indep(x, y))
				f.u(x, y) = vec2d(0, 0);
		}
	}
}

void SimulatorClassic::enforceBoundary(SimFrame& f)
{
	enforcePBoundary(f);
	enforceUBoundary(f);
}

void SimulatorClassic::iter()
{
	f0 = f1;
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (indep(x, y))
				field(x, y) = -(f0.u(x + 1, y).x - f0.u(x - 1, y).x + f0.u(x, y + 1).y - f0.u(x, y - 1).y) / dt / (2 * dx)
					+ (f0.u(x + 1, y).x - f0.u(x - 1, y).x) * (f0.u(x + 1, y).x - f0.u(x - 1, y).x) / 4 / (dx * dx)
					+ 2 * (f0.u(x, y + 1).x - f0.u(x, y - 1).x) / 2 / dx * (f0.u(x + 1, y).y - f0.u(x - 1, y).y) / 2 / dx
					+ (f0.u(x, y + 1).y - f0.u(x, y - 1).y) * (f0.u(x, y + 1).y - f0.u(x, y - 1).y) / 4 / (dx * dx);
		}
	}
	while (true)
	{
		f0.p = f1.p;
		double dl1 = 0;
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y))
				{
					f1.p(x, y) = (f0.p(x + 1, y) + f0.p(x - 1, y) + f0.p(x, y + 1) + f0.p(x, y - 1) + (dx * dx) * field(x, y)) / 4;
					dl1 += abs(f1.p(x, y) - f0.p(x, y));
				}
			}
		}
		enforcePBoundary(f1);
		if (dl1 < lapL1limit)
			break;
	}
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (indep(x, y))
			{
				f1.u(x, y).x = f0.u(x, y).x
					+ dt * (
						-f0.u(x, y).x * (f0.u(x + 1, y).x - f0.u(x - 1, y).x) / 2 / dx - f0.u(x, y).y * (f0.u(x, y + 1).x - f0.u(x, y - 1).x) / 2 / dx
						- (f0.p(x + 1, y) - f0.p(x - 1, y)) / 2 / dx
						+ nu * (f0.u(x + 1, y).x + f0.u(x - 1, y).x + f0.u(x, y + 1).x + f0.u(x, y - 1).x - 4 * f0.u(x, y).x) / (dx * dx)
					);
				f1.u(x, y).y = f0.u(x, y).y
					+ dt * (
						-f0.u(x, y).x * (f0.u(x + 1, y).y - f0.u(x - 1, y).y) / 2 / dx - f0.u(x, y).y * (f0.u(x, y + 1).y - f0.u(x, y - 1).y) / 2 / dx
						- (f0.p(x, y + 1) - f0.p(x, y - 1)) / 2 / dx
						+ nu * (f0.u(x + 1, y).y + f0.u(x - 1, y).y + f0.u(x, y + 1).y + f0.u(x, y - 1).y - 4 * f0.u(x, y).y) / (dx * dx)
					);
			}
		}
	}
	enforceBoundary(f1);
}

}