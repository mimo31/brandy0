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
	PLAY_UNTIL_END, LOOP, LAST_FRAME_ONLY
};

class SimulationStateAbstr
{
public:
	static constexpr double MAX_PLAYBACK_SPEEDUP = 10000;
	static constexpr double MS_PER_BASE_FRAME = 40;
	static const PlaybackMode defaultPlaybackMode = PlaybackMode::PLAY_UNTIL_END;
	static constexpr uint32_t
		DEFAULT_VIDEO_WIDTH = 1920, MAX_VIDEO_WIDTH = 8192,
		DEFAULT_VIDEO_HEIGHT = 1080, MAX_VIDEO_HEIGHT = 8192,
		DEFAULT_VIDEO_BITRATE = 320'000, MAX_VIDEO_BITRATE = 4'000'000;
	
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
	ValidatorManager entryFieldValidators;

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

	virtual bool isComputing() = 0;
	virtual uint32_t getFramesStored() = 0;
	virtual uint32_t getComputedIter() = 0;

	virtual void videoExportValidateRange() = 0;
	virtual void videoExportClampTime() = 0;

	SimulationStateAbstr(ApplicationAbstr *const app);
};

}

#endif // SIMULATION_STATE_ABSTR_HPP