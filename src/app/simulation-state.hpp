/**
 * simulation-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIMULATION_STATE_HPP
#define SIMULATION_STATE_HPP

#include <thread>

#include <gtkmm/application.h>

#include "glob.hpp"

#include "application.hpp"
#include "config-window.hpp"
#include "simulation-window.hpp"
#include "simulator.hpp"
#include "simulator-params.hpp"
#include "state.hpp"

namespace brandy0
{

class Application;

template<typename T>
using vec = std::vector<T>;

class SimulationState : public State
{
private:
	Application *app;
	SimulationWindow *win;
	SimulatorParams *params;
	Simulator *sim;
	vec<SimFrame> frames;
	std::thread computeThread;
	std::thread redrawThread;
	bool computing;
	bool stopComputingSignal = false;
	std::mutex computingMutex;
	std::mutex framesMutex;
	uint32_t frameCount = 0;
	uint32_t frameStepSize = 1;
	uint32_t drawFrame = 0;

	void setParams(const SimulatorParams&);
	void showWindow();
	void checkCapacity();
	void addLastFrame();
	double getTime(const uint32_t frame);
	void runComputeThread();
	void startComputeThread();
	bool runRedrawThread();
	void startRedrawThread();
public:
	SimulationState(Application *const);
	void activate(const SimulatorParams&);
	void deactivate();
	~SimulationState();
};

}

#endif // SIMULATION_STATE_HPP
