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

#include "application-abstr.hpp"
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

/**
 * Class representing the application's simulation state
 */
class SimulationState : public State, public SimulationStateAbstr
{
private:
	/// Main simulation window
	SimulationWindow mainWin;
	/// Video export window
	ExportWindow exportWin;

	/// Used simulator. May be null if the simulation state is not active at the moment
	uptr<Simulator> sim;
	/// Vector of all computed simulation frames. @see frameStepSize for the time interval between two consecutive frames in this vector
	vec<SimFrame> frames;
	/// Thread running the simulation computation
	std::thread computeThread;
	/// True iff the compute thread is running. Used in multiple threads, guarded by computingMutex
	bool computing;
	/**
	 * True iff the compute thread should promptly stop. The compute thread should set this back to false after terminating due to it.
	 * Used in multiple threads, guarded by computeMutex.
	 */
	bool stopComputingSignal = false;
	/// Mutex guarding variables related to the simulation computation that are used in multiple threads
	std::mutex computingMutex;
	/// Mutex guarding the frames vector and other variables related to the stored frames
	std::mutex framesMutex;
	/// Mutex guarding the crashSignal variable
	std::mutex crashMutex;
	/// True iff the computation thread is signalling that the simulation has crashed
	bool crashSignal;

	/**
	 * The number of computed base frames
	 * (may be smaller than frames.size() if the frame capacity has already been reached and frames are stored less frequently)
	 */
	uint32_t frameCount;
	/**
	 * The number of base frames corresponing to one current frame.
	 * Always a power of 2
	 */
	uint32_t frameStepSize;
	/// Number of computed iterations in the current frame last time this variable was update
	uint32_t computedIter;
	/// Timer which periodically recalculates current values (e.g. advances time) and requests redraws of the display area
	sigc::connection updateConnection;

	/**
	 * Number of base frames that should be computed before the application automatically exits
	 * 0 iff the application should not automatically exit
	 */
	uint32_t closeAfterFrames = 0;

	/**
	 * Shows the main simulation window
	 */	
	void showMainWindow();
	/**
	 * Shows the video export window
	 */
	void showExportWindow();
	/**
	 * Checks whether the frames vector has reached its full capacity as specified in the simulation parameters.
	 * If it has, deletes every other frame and increases the frame step size
	 */
	void checkCapacity();
	/**
	 * Increments frameCount and if the current frame should be stored, reads it from the simulator and stores it in the frames vector
	 */
	void addLastFrame();
	/**
	 * @param number zero-indexed number of a base frame
	 * @return simulation time of the specified base frame
	 */
	double getTime(uint32_t frame);
	/**
	 * Calls the iterations in the simulator in a loop until there is some indication to stop (e.g. the stop computing signal)
	 */
	void runComputeThread();
	/**
	 * Creates a new thread that runs the simulation computation
	 */
	void startComputeThread();
	/**
	 * Updates the status variables based on elapsed time and the state of the simulator
	 * @return true (this method is passed as a callback to a timer)
	 */
	bool update();

	/**
	 * Initializes attributes to prepare for the start of a simulation
	 * @param params simulation parameters for the new simulation
	 */
	void start(const SimulationParams& params);

	/**
	 * Updates the computedTime attribute based on other attributes (especially the size of the frames vector)
	 */
	void updateComputedTime();

	/**
	 * Gets the index of the frame in the frames vector that should be displayed at the specified simulation time
	 * @param t simulation time for which a frame should be found
	 * @return index of the frame in the frames vector that should be displayed at the specified time
	 */
	uint32_t getFrameNumber(double t);
	/**
	 * Time point of the last update (of state attributes based on but outside the computation thread)
	 */
	std::chrono::steady_clock::time_point lastUpdate;

public:
	/**
	 * Constructs a SimulationState object
	 * @param app pointer to the parent (abstract) application
	 */
	SimulationState(ApplicationAbstr *app);
	/**
	 * Activates the state to run a simulation with specified parameters
	 * @param params simulation parameters to run the simulation with
	 */
	void activate(const SimulationParams &params);
	/**
	 * Runs the parent application with the main simulation window and exits after computing the specified number of frames
	 * @param params simulation parameters to run the simulation with
	 * @param frames number of frames to compute before automatically exiting
	 */
	void run(const SimulationParams &params, uint32_t frames);
	void deactivate() override;
	
	void goBackToConfig() override;
	void pauseComputation() override;
	void resumeComputation() override;
	void closeAll() override;
	void enterVideoExport() override;
	void leaveVideoExport() override;
	void confirmVideoExport() override;
	void cancelVideoExport() override;

	bool isComputing() override;
	uint32_t getFramesStored() override;
	uint32_t getComputedIter() override;

	void videoExportValidateRange() override;
	void videoExportClampTime() override;
};

}

#endif // SIMULATION_STATE_HPP
