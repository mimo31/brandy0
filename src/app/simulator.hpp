/**
 * simulator.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/10/29
 */
#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <mutex>

#include "glob.hpp"

#include "grid.hpp"
#include "sim-frame.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

class Simulator
{
public:
	double w, h;
	bool crashed, incomplete;
	SimFrame f0, f1;

	const bool *pauseSignal = nullptr;
	std::mutex *controlMutex = nullptr;
	
	Simulator(const SimulationParams& params);
	virtual void iter() = 0;
	void setPauseControl(const bool *pauseSignal, std::mutex *controlMutex);
	virtual ~Simulator() {}

protected:
	double dt;
	double dx, dy;
	uint32_t wp, hp;
	double rho, mu, nu;
	Grid<bool> solid;
	Grid<bool> indep;
	BoundaryCond bcx0, bcx1, bcy0, bcy1;
	vec2d to_coor(const Point&) const;
	vec2d to_coor(int32_t x, int32_t y) const;
};

}

#endif // SIMULATOR_HPP
