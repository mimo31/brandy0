/**
 * simulator.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <mutex>

#include "grid.hpp"
#include "sim-frame.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

/**
 * Class providing an abstract interface for the simulators that compute the simulations
 */
class Simulator
{
public:
	/// Physical width of the simulated container
	double w;
	/// Physical height of the simulated container
	double h;
	/// True iff the simulation has crashed
	bool crashed;
	/// True iff the simulation has been interrupted (paused) during the computation of a frame
	bool incomplete;
	/// Previous simulation frame (one before the most current one)
	SimFrame f0;
	/// Most current (latest computed) simulation frame
	SimFrame f1;
	
	/**
	 * Constructs the Simulator objects for specified simulation parameters
	 * @param params simulation parameters for the simulation
	 */
	Simulator(const SimulationParams &params);
	/**
	 * Computes the next frame of the simulation (and stores it in the f1 attribute)
	 */
	virtual void iter() = 0;
	/**
	 * Sets what variable and mutex should be used for pause signalling
	 * @param pauseSignal pointer to the variable which signals pause
	 * @param controlMutex pointer to the mutex guarding the signalling variable
	 */
	void setPauseControl(const bool *pauseSignal, std::mutex *controlMutex);
	virtual ~Simulator() {}

protected:
	/// Pointer to a boolean variable which is true iff the simulator should promptly pause (i.e. exit the iter method)
	const bool *pauseSignal = nullptr;
	/// Mutex guarding the variable that pauseSignal points to
	std::mutex *controlMutex = nullptr;

	/// Simulation time step (dt)
	double dt;
	/// Simulation spacial step along the x axis (dx)
	double dx;
	/// Simulation spacial step along the y axis (dy)
	double dy;
	/// Number of grid points along the x axis
	uint32_t wp;
	/// Number of grid points along the y axis
	uint32_t hp;
	/// Fluid density (rho)
	double rho;
	/// Fluid dynamic viscosity (mu)
	double mu;
	/// Fluid kinematic viscosity (nu)
	double nu;
	/// Grid in which a point is true iff not occupied by an obstacle
	Grid<bool> solid;
	/// Grid in which a point is true iff not solid and not at the boundary (i.e. it and all its 4 direct neighbors are non-solid) 
	Grid<bool> indep;
	/// Left boundary condition
	BoundaryCond bcx0;
	/// Right boundary condition
	BoundaryCond bcx1;
	/// Bottom boundary condition
	BoundaryCond bcy0;
	/// Top boundary condition
	BoundaryCond bcy1;
};

}

#endif // SIMULATOR_HPP
