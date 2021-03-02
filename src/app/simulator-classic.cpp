/**
 * simulator-classic.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/26
 */
#include "simulator-classic.hpp"

namespace brandy0
{

void SimulatorClassic::visit(const Point p)
{
	if (visited(p))
		return;
	visited(p) = true;
	if (p.x != 0 && indep(p.x - 1, p.y))
		visit(Point(p.x - 1, p.y));
	if (uint32_t(p.x + 1) != wp && indep(p.x + 1, p.y))
		visit(Point(p.x + 1, p.y));
	if (p.y != 0 && indep(p.x, p.y - 1))
		visit(Point(p.x, p.y - 1));
	if (uint32_t(p.y + 1) != hp && indep(p.x, p.y + 1))
		visit(Point(p.x, p.y + 1));
}

SimulatorClassic::SimulatorClassic(const SimulatorParams& params)
	: Simulator(params), field(wp, hp), dirichlet(wp, hp), visited(wp, hp), lapL1limit(.1 * wp * hp / 64 / 64), crashLimit(1e13)
{
	dirichlet.set_all(false);
	if (bcx0.p == PressureBoundaryCond::DIRICHLET)
		for (uint32_t y = 0; y < hp; y++)
			dirichlet(0, y) = true;
	if (bcx1.p == PressureBoundaryCond::DIRICHLET)
		for (uint32_t y = 0; y < hp; y++)
			dirichlet(wp - 1, y) = true;
	if (bcy0.p == PressureBoundaryCond::DIRICHLET)
		for (uint32_t x = 0; x < wp; x++)
			dirichlet(x, 0) = true;
	if (bcy1.p == PressureBoundaryCond::DIRICHLET)
		for (uint32_t x = 0; x < wp; x++)
			dirichlet(x, hp - 1) = true;
	
	visited.set_all(false);
	for (uint32_t y = 0; y < hp; y++)
	{
		for (uint32_t x = 0; x < wp; x++)
		{
			if (dirichlet(x, y))
				visit(Point(x, y));
			else if (!indep(x, y))
				visited(x, y) = true;
		}
	}
	for (uint32_t y = 0; y < hp; y++)
	{
		for (uint32_t x = 0; x < wp; x++)
		{
			if (!visited(x, y))
			{
				dirichlet(x, y) = true;
				visit(Point(x, y));
			}
		}
	}
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
			if (dirichlet(x, y))
			{
				f.p(x, y) = 0;
				continue;
			}
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
	if (crashed)
		return;
	if (!incomplete)
	{
		f0 = f1;
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y))
				{
					field(x, y) = -(f0.u(x + 1, y).x - f0.u(x - 1, y).x + f0.u(x, y + 1).y - f0.u(x, y - 1).y) / dt / (2 * dx)
						+ (f0.u(x + 1, y).x - f0.u(x - 1, y).x) * (f0.u(x + 1, y).x - f0.u(x - 1, y).x) / 4 / (dx * dx)
					+ 2 * (f0.u(x, y + 1).x - f0.u(x, y - 1).x) / 2 / dx * (f0.u(x + 1, y).y - f0.u(x - 1, y).y) / 2 / dx
					+ (f0.u(x, y + 1).y - f0.u(x, y - 1).y) * (f0.u(x, y + 1).y - f0.u(x, y - 1).y) / 4 / (dx * dx);
					if (field(x, y) > crashLimit || field(x, y) < -crashLimit)
					{
						crashed = true;
						return;
					}
				}
			}
		}
	}
	incomplete = false;
	uint32_t it = 0;
	while (true)
	{
		f0.p = f1.p;
		double dl1 = 0;
		cout.precision(15);
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y) && !dirichlet(x, y))
				{
					double sm = 0;
					uint32_t coef = 4;
					if (!dirichlet(x + 1, y))
					{
						if (indep(x + 1, y))
							sm += f0.p(x + 1, y);
						else
							coef--;
					}
					if (!dirichlet(x - 1, y))
					{
						if (indep(x - 1, y))
							sm += f0.p(x - 1, y);
						else
							coef--;
					}
					if (!dirichlet(x, y + 1))
					{
						if (indep(x, y + 1))
							sm += f0.p(x, y + 1);
						else
							coef--;
					}
					if (!dirichlet(x, y - 1))
					{
						if (indep(x, y - 1))
							sm += f0.p(x, y - 1);
						else
							coef--;
					}
					f1.p(x, y) = (sm + (dx * dx) * field(x, y)) / coef;
					dl1 += abs(f1.p(x, y) - f0.p(x, y));
				}
			}
		}
		if (std::isnan(dl1))
		{
			crashed = true;
			return;
		}
		enforcePBoundary(f1);
		if (dl1 < lapL1limit)
			break;
		it++;
		if (it == 1000)
		{
			it = 0;
			if (controlMutex)
			{
				controlMutex->lock();
				if (*pauseSignal)
				{
					incomplete = true;
					controlMutex->unlock();
					return;
				}
				controlMutex->unlock();
			}
		}
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
				if (std::isnan(f1.u(x, y).x) || std::isnan(f1.u(x, y).y))
				{
					crashed = true;
					return;
				}
			}
		}
	}
	enforceBoundary(f1);
}

}