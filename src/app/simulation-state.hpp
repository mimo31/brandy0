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
#include "export-window.hpp"
#include "simulation-state-abstr.hpp"
#include "simulation-window.hpp"
#include "simulator.hpp"
#include "simulator-params.hpp"
#include "state.hpp"

namespace brandy0
{

template<typename T>
using vec = std::vector<T>;

class SimulationState : public State, public SimulationStateAbstr
{
private:
	ApplicationAbstr *app;
	std::unique_ptr<SimulationWindow> mainWin;
	std::unique_ptr<ExportWindow> exportWin;
	std::unique_ptr<Simulator> sim;
	vec<SimFrame> frames;
	std::thread computeThread;
	std::thread redrawThread;
	bool computing;
	bool stopComputingSignal = false;
	std::mutex computingMutex;
	std::mutex framesMutex;
	uint32_t frameCount;
	uint32_t frameStepSize;
	uint32_t computedIter;
	sigc::connection redrawConnection;

	void showMainWindow();
	void showExportWindow();
	void checkCapacity();
	void addLastFrame();
	double getTime(const uint32_t frame);
	void runComputeThread();
	void startComputeThread();
	bool update();

	void updateComputedTime();

public:
	SimulationState(ApplicationAbstr *const);
	void activate(const SimulatorParams&);
	void deactivate();
	
	void goBackToConfig() override;
	void pauseComputation() override;
	void resumeComputation() override;
	void enterVideoExport() override;
	void leaveVideoExport() override;

	bool isComputing() override;
	uint32_t getFramesStored() override;
	uint32_t getComputedIter() override;
};

}

#endif // SIMULATION_STATE_HPP
