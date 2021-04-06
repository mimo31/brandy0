/**
 * simulator-classic.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/26
 */
#ifndef SIMULATOR_CLASSIC_HPP
#define SIMULATOR_CLASSIC_HPP

#include "simulator.hpp"

namespace brandy0
{

class SimulatorClassic : public Simulator
{
private:
	/// Grid for the 'w' field (divergence of the intermediate velocity)
	Grid<vec2d> ww;
	/// Grid for the RHS of the Poisson equation for pressure
    Grid<double> field;
	/**
	 * Grid in which a point is true iff the value of pressure there is set directly by a Dirichlet boundary condition
	 * (or arbitrarily because it didn't connect to any point with set pressure and the pressure field is determined up to a constant)
	 */
	Grid<bool> dirichlet;
	/**
	 * Grid marking points as visited for the purposes of a search that identifies isolated components to set constant pressure values there
	 */
	Grid<bool> visited;

	/**
	 * Marks the specified point as visited and calls itself on its (direct -- at most 4) neighbors.
	 * Does nothing if the specified point was already marked as visited.
	 * @param p grid point to visit
	 */
	void visit(Point p);

	/// Upper bound on the L1 norm of the change of the pressure field for the solving of the Poisson equation for pressure to stop
    double lapL1limit;
	/// Upper bound for the value of the RHS in the Poisson equation for pressure at any point such that the simulation is not declared as divergent (crashed)
	double crashLimit;

	/**
	 * Modifies the specified pressure field to comply with the boundary conditions for pressure
	 * @param p pressure field to modify
	 */
    void enforcePBoundary(Grid<double> &p);
	/**
	 * Modifies the specified velocity field to comply with the boundary conditions for velocity
	 * @param u velocity field to modify
	 */
    void enforceUBoundary(Grid<vec2d> &u);
	/**
	 * Modifies both the pressure and the velocity field of a simulation frame to comply with the boundary conditions
	 * @param f simulation frame to modify
	 */
    void enforceBoundary(SimFrame &f);
public:
	/**
	 * Constructs a SimulatorClassic object
	 */
	SimulatorClassic(const SimulationParams &params);
    void iter() override;
};

}


#endif // SIMULATOR_CLASSIC_HPP