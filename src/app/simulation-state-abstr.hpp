/**
 * simulation-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/18
 */
#ifndef SIMULATION_STATE_ABSTR_HPP
#define SIMULATION_STATE_ABSTR_HPP

#include "application-abstr.hpp"
#include "listener-manager.hpp"
#include "ptr.hpp"
#include "sim-frame.hpp"
#include "simulation-params.hpp"
#include "str.hpp"
#include "validator-manager.hpp"
#include "video-exporter.hpp"

namespace brandy0
{

enum PlaybackMode
{
	PlayUntilEnd, Loop, LastFrameOnly
};

class SimulationStateAbstr
{
public:
	static constexpr double MsPerBaseFrame = 40;
	static const PlaybackMode defaultPlaybackMode = PlaybackMode::PlayUntilEnd;
	static constexpr uint32_t
		DefaultVideoWidth = 1920, MaxVideoWidth = 8192,
		DefaultVideoHeight = 1080, MaxVideoHeight = 8192,
		DefaultVideoBitrate = 320'000, MaxVideoBitrate = 4'000'000;
	
	ApplicationAbstr *app;

	uptr<SimulationParams> params;
	uptr<SimFrame> curFrame;
	
	ListenerManager initListeners;
	ListenerManager computingSwitchListeners;
	ListenerManager playbackStateChangeListeners;
	ListenerManager playbackModeChangeListeners;
	ListenerManager updateListeners;
	ListenerManager closeListeners;
	ListenerManager crashListeners;

	ListenerManager vexpEnterListeners;
	ListenerManager vexpLeaveListeners;
	ListenerManager vexpStartTimeChangeListeners;
	ListenerManager vexpEndTimeChangeListeners;
	ListenerManager vexpPlaybackSpeedupChangeListeners;
	ListenerManager vexpRangeValidityChangeListeners;
	ListenerManager vexpPlaybackStateChangeListeners;
	ListenerManager vexpFileLocationChangeListeners;
	ListenerManager vexpEntryValidityChangeListeners;
	ListenerManager vexpExportUpdateListeners;
	ListenerManager vexpExportStateChangeListeners;
	ValidatorManager vexpEntryFieldValidators;

	double time;
	double computedTime;
	bool crashed;

	double videoExportTime;
	double videoExportStartTime;
	double videoExportEndTime;
	double videoExportPlaybackSpeedup;
	bool videoExportRangeValid;
	bool videoExportEditingTime;
	bool videoExportPlaybackPaused;
	str videoExportFileLocation;
	uint32_t videoExportWidth, videoExportHeight;
	uint32_t videoExportBitrate;

	uint32_t backDisplayMode;
	uint32_t frontDisplayMode;

	bool editingTime;

	bool playbackPaused;

	PlaybackMode playbackMode;

	double playbackSpeedup;

	bool inVideoExport;
	uptr<VideoExporter> videoExporter;

	virtual void goBackToConfig() = 0;
	virtual void pauseComputation() = 0;
	virtual void resumeComputation() = 0;

	virtual void closeAll() = 0;

	virtual void enterVideoExport() = 0;
	virtual void leaveVideoExport() = 0;
	virtual void confirmVideoExport() = 0;
	virtual void cancelVideoExport() = 0;

	virtual bool isComputing() = 0;
	virtual uint32_t getFramesStored() = 0;
	virtual uint32_t getComputedIter() = 0;

	virtual void videoExportValidateRange() = 0;
	virtual void videoExportClampTime() = 0;

	SimulationStateAbstr(ApplicationAbstr *app);
};

}

#endif // SIMULATION_STATE_ABSTR_HPP