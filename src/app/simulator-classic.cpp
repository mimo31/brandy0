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

SimulatorClassic::SimulatorClassic(const SimulationParams& params)
	: Simulator(params), ww(wp, hp), field(wp, hp), dirichlet(wp, hp), visited(wp, hp), lapL1limit(.001 * wp * hp / 64 / 64), crashLimit(1e13)
{
	dirichlet.set_all(false);
	if (bcx0.ptype == BoundaryCondType::Dirichlet)
		for (uint32_t y = 0; y < hp; y++)
			dirichlet(0, y) = true;
	if (bcx1.ptype == BoundaryCondType::Dirichlet)
		for (uint32_t y = 0; y < hp; y++)
			dirichlet(wp - 1, y) = true;
	if (bcy0.ptype == BoundaryCondType::Dirichlet)
		for (uint32_t x = 0; x < wp; x++)
			dirichlet(x, 0) = true;
	if (bcy1.ptype == BoundaryCondType::Dirichlet)
		for (uint32_t x = 0; x < wp; x++)
			dirichlet(x, hp - 1) = true;
	
	// search the grid as a graph and for each connected component without any points with
	// pressure set by a Dirichlet b.c., choose one that will be set that way
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

void SimulatorClassic::enforcePBoundary(Grid<double>& p)
{
	if (bcx0.ptype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t y = 0; y < hp; y++)
			p(0, y) = bcx0.p;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			p(0, y) = p(1, y);
	}
	if (bcx1.ptype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t y = 0; y < hp; y++)
			p(wp - 1, y) = bcx1.p;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			p(wp - 1, y) = p(wp - 2, y);
	}
	if (bcy0.ptype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t x = 0; x < wp; x++)
			p(x, 0) = bcy0.p;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			p(x, 0) = p(x, 1);
	}
	if (bcy1.ptype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t x = 0; x < wp; x++)
			p(x, hp - 1) = bcy1.p;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			p(x, hp - 1) = p(x, hp - 2);
	}
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (dirichlet(x, y))
			{
				p(x, y) = 0;
				continue;
			}
			if (solid(x, y) || indep(x, y))
				continue;
			
			// average the pressure from neighbors
			uint32_t cou = 0;
			double sm = 0;
			if (indep(x - 1, y))
			{
				sm += p(x - 1, y);
				cou++;
			}
			if (indep(x + 1, y))
			{
				sm += p(x + 1, y);
				cou++;
			}
			if (indep(x, y - 1))
			{
				sm += p(x, y - 1);
				cou++;
			}
			if (indep(x, y + 1))
			{
				sm += p(x, y + 1);
				cou++;
			}
			p(x, y) = cou != 0 ? sm / cou : 0;
		}
	}
}

void SimulatorClassic::enforceUBoundary(Grid<vec2d>& u)
{
	if (bcx0.utype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t y = 0; y < hp; y++)
			u(0, y) = bcx0.u;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			u(0, y) = u(1, y);
	}
	if (bcx1.utype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t y = 0; y < hp; y++)
			u(wp - 1, y) = bcx1.u;
	}
	else
	{
		for (uint32_t y = 0; y < hp; y++)
			u(wp - 1, y) = u(wp - 2, y);
	}
	if (bcy0.utype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t x = 0; x < wp; x++)
			u(x, 0) = bcy0.u;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			u(x, 0) = u(x, 1);
	}
	if (bcy1.utype == BoundaryCondType::Dirichlet)
	{
		for (uint32_t x = 0; x < wp; x++)
			u(x, hp - 1) = bcy1.u;
	}
	else
	{
		for (uint32_t x = 0; x < wp; x++)
			u(x, hp - 1) = u(x, hp - 2);
	}

	// at obstacle boundaries, the velocity is given as (0, 0) by the no-slip condition
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (!solid(x, y) && !indep(x, y))
				u(x, y) = vec2d(0, 0);
		}
	}
}

void SimulatorClassic::enforceBoundary(SimFrame& f)
{
	enforcePBoundary(f.p);
	enforceUBoundary(f.u);
}

void SimulatorClassic::iter()
{
	if (crashed)
		return;
	if (!incomplete)
	{
		f0 = f1;
		// compute the w field
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y))
				{
					// viscous term (laplacian of u)
					const vec2d lapu = (f0.u(x + 1, y) - 2 * f0.u(x, y) + f0.u(x - 1, y)) / (dx * dx)
						+ (f0.u(x, y + 1) - 2 * f0.u(x, y) + f0.u(x, y - 1)) / (dy * dy);
					/*const vec2d convec = f0.u(x, y).x * (f0.u(x + 1, y) - f0.u(x - 1, y)) / (2 * dx)
						+ f0.u(x, y).y * (f0.u(x, y + 1) - f0.u(x, y - 1)) / (2 * dy);*/
					// convective term of u (using upwind differencing)
					const vec2d convec = f0.u(x, y).x * (f0.u(x, y).x > 0 ? f0.u(x, y) - f0.u(x - 1, y) : f0.u(x + 1, y) - f0.u(x, y)) / dx
						+ f0.u(x, y).y * (f0.u(x, y).y > 0 ? f0.u(x, y) - f0.u(x, y - 1) : f0.u(x, y + 1) - f0.u(x, y)) / dy;
					ww(x, y) = f0.u(x, y) + dt * (nu * lapu - convec);
				}
			}
		}
		enforceUBoundary(ww);
		// compute the RHS of the Poisson equation for pressure
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y))
				{
					field(x, y) = rho / dt * ((ww(x + 1, y).x - ww(x - 1, y).x) / (2 * dx) + (ww(x, y + 1).y - ww(x, y - 1).y) / (2 * dy)); 
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
	// solve the Poisson equation for pressure
	while (true)
	{
		//f0.p = f1.p;
		double dl1 = 0;
		for (uint32_t y = 1; y < hp - 1; y++)
		{
			for (uint32_t x = 1; x < wp - 1; x++)
			{
				if (indep(x, y) && !dirichlet(x, y))
				{
					double sm = 0;
					double coef = 2 * dx * dx + 2 * dy * dy;
					if (indep(x + 1, y) || dirichlet(x + 1, y))
						sm += dy * dy * f1.p(x + 1, y);
					else
						coef -= dy * dy;
					if (indep(x - 1, y) || dirichlet(x - 1, y))
						sm += dy * dy * f1.p(x - 1, y);
					else
						coef -= dy * dy;
					if (indep(x, y + 1) || dirichlet(x, y + 1))
						sm += dx * dx * f1.p(x, y + 1);
					else
						coef -= dx * dx;
					if (indep(x, y - 1) || dirichlet(x, y - 1))
						sm += dx * dx * f1.p(x, y - 1);
					else
						coef -= dx * dx;
					const double newval = (sm - (dx * dx) * (dy * dy) * field(x, y)) / coef * 1.5 - .5 * f1.p(x, y);
					dl1 += abs(f1.p(x, y) - newval);
					f1.p(x, y) = newval;
				}
			}
		}
		if (std::isnan(dl1))
		{
			crashed = true;
			return;
		}
		enforcePBoundary(f1.p);
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
	// update the velocity field using the computed pressure
	for (uint32_t y = 1; y < hp - 1; y++)
	{
		for (uint32_t x = 1; x < wp - 1; x++)
		{
			if (indep(x, y))
			{
				f1.u(x, y).x = ww(x, y).x - dt / rho * (f1.p(x + 1, y) - f1.p(x - 1, y)) / (2 * dx);
				f1.u(x, y).y = ww(x, y).y - dt / rho * (f1.p(x, y + 1) - f1.p(x, y - 1)) / (2 * dy);
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