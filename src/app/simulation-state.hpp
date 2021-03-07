/**
 * simulation-state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIMULATION_STATE_HPP
#define SIMULATION_STATE_HPP

#include <chrono>
#include <thread>

#include <gtkmm/application.h>

#include "glob.hpp"

#include "application.hpp"
#include "config-window.hpp"
#include "export-window.hpp"
#include "ptr.hpp"
#include "simulation-state-abstr.hpp"
#include "simulation-window.hpp"
#include "simulator.hpp"
#include "simulation-params.hpp"
#include "state.hpp"
#include "vec.hpp"

namespace brandy0
{

class SimulationState : public State, public SimulationStateAbstr
{
private:
	ApplicationAbstr *app;
	uptr<SimulationWindow> mainWin;
	uptr<ExportWindow> exportWin;
	uptr<Simulator> sim;
	vec<SimFrame> frames;
	std::thread computeThread;
	std::thread redrawThread;
	bool computing;
	bool stopComputingSignal = false;
	std::mutex computingMutex, framesMutex, crashMutex;
	bool crashSignal;
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

	uint32_t getFrameNumber(const double t);
	std::chrono::steady_clock::time_point lastUpdate;

public:
	SimulationState(ApplicationAbstr *const);
	void activate(const SimulationParams&);
	void deactivate();
	
	void goBackToConfig() override;
	void pauseComputation() override;
	void resumeComputation() override;
	void closeAll() override;
	void enterVideoExport() override;
	void leaveVideoExport() override;
	void confirmVideoExport() override;

	bool isComputing() override;
	uint32_t getFramesStored() override;
	uint32_t getComputedIter() override;

	void videoExportValidateRange() override;
	void videoExportClampTime() override;

	~SimulationState() { };
};

}

#endif // SIMULATION_STATE_HPP
