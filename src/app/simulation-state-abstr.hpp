/**
 * simulation-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/18
 */
#ifndef SIMULATION_STATE_ABSTR_HPP
#define SIMULATION_STATE_ABSTR_HPP

#include <memory>

#include "listener-manager.hpp"
#include "sim-frame.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

enum PlaybackMode
{
	PLAY_UNTIL_END, LOOP, LAST_FRAME_ONLY
};

class SimulationStateAbstr
{
public:
	static const PlaybackMode defaultPlaybackMode = PlaybackMode::PLAY_UNTIL_END;
	std::unique_ptr<SimulatorParams> params;
	std::unique_ptr<SimFrame> curFrame;

	ListenerManager initListeners;
	ListenerManager computingSwitchListeners;
	ListenerManager playbackStateChangeListeners;
	ListenerManager playbackModeChangeListeners;
	ListenerManager closeListeners;

	double time;
	double computedTime;

	uint32_t backDisplayMode;
	uint32_t frontDisplayMode;

	bool editingTime;

	bool playbackPaused;

	PlaybackMode playbackMode;

	double playbackSpeedup;

	virtual void goBackToConfig() = 0;
	virtual void pauseComputation() = 0;
	virtual void resumeComputation() = 0;
	virtual bool isComputing() = 0;
	virtual uint32_t getFramesStored() = 0;
	virtual uint32_t getComputedIter() = 0;
};

}

#endif // SIMULATION_STATE_ABSTR_HPP