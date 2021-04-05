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

/**
 * Enum representing possible modes of computed simulation playback
 */
enum PlaybackMode
{
	/**
	 * Mode in which the computed simulation is played at the set speed.
	 * Whenever the time to be displayed reaches beyond the computed simulation end, it is set to the time of the computed simulation end.
	 */
	PlayUntilEnd,
	/**
	 * Mode in which the computed simulation is played at the set speed.
	 * Whenever the time to be displayed reaches beyond the computed simulation end, it loops back to the start of the simulation
	 */
	Loop,
	/**
	 * Mode in which only the last computed frame is displayed at all times
	 */
	LastFrameOnly
};

/**
 * The abstract interface for the simulation state for its child windows
 * (@see ApplicationAbstr for more explanation on the *Abstr classes)
 */
class SimulationStateAbstr
{
public:
	/// Number of milliseconds between two consequtive base frames
	static constexpr double MsPerBaseFrame = 40;
	/// Default computed simulation playback mode
	static const PlaybackMode defaultPlaybackMode = PlaybackMode::PlayUntilEnd;
	/// Default width of exported video
	static constexpr uint32_t DefaultVideoWidth = 1920;
	/// Maximum width of exported video
	static constexpr uint32_t MaxVideoWidth = 8192;
	/// Default height of exported video
	static constexpr uint32_t DefaultVideoHeight = 1080;
	/// Maximum height of exported video
	static constexpr uint32_t MaxVideoHeight = 8192;
	/// Default bitrate of exported video
	static constexpr uint32_t DefaultVideoBitrate = 320'000;
	/// Maximum bitrate of exported video
	static constexpr uint32_t MaxVideoBitrate = 4'000'000;
	
	/// Pointer to parent application
	ApplicationAbstr *app;

	/// Simulation parameters of the current simulation
	uptr<SimulationParams> params;
	/// Frame that should be currently drawn in the DisplayArea (that should be drawn when a redraw is issued)
	uptr<SimFrame> curFrame;
	
	/// Listeners for the event when the state has been activated and its attributes have been set
	ListenerManager initListeners;
	/// Listeners for the event when the state of the computation changes (it gets paused or resumed)
	ListenerManager computingSwitchListeners;
	/// Listeners for the event when the state of the playback changes (from playing to paused or vice versa)
	ListenerManager playbackStateChangeListeners;
	/// Listeners for the event when the simulation playback mode changes
	ListenerManager playbackModeChangeListeners;
	/// Listeners for frequent, periodic updates of the state
	ListenerManager updateListeners;

	/// Listeners for the event when a crash of the simulation has been detected
	ListenerManager crashListeners;

	/// Listeners for the event when the video export dialog has been opened
	ListenerManager vexpEnterListeners;
	/// Listeners for the event when the video export dialog has been closed
	ListenerManager vexpLeaveListeners;
	/// Listeners for the event when the start time in the video export dialog has changed
	ListenerManager vexpStartTimeChangeListeners;
	/// Listeners for the event when the end time in the video export dialog has changed
	ListenerManager vexpEndTimeChangeListeners;
	/// Listeners for the event when the playback speed in the video export dialog has changed
	ListenerManager vexpPlaybackSpeedupChangeListeners;
	/// Listeners for the event when the validity of the video export time range has changed
	ListenerManager vexpRangeValidityChangeListeners;
	/// Listeners for the event when the state of the preview playback in video export has changed (from paused to playing or vice versa)
	ListenerManager vexpPlaybackStateChangeListeners;
	/// Listeners for the event when the video export filename has changed
	ListenerManager vexpFileLocationChangeListeners;
	/// Listeners for the event when the validity of the annotated entries in the video export dialog may have changed
	ListenerManager vexpEntryValidityChangeListeners;
	/// Listeners for the export progress updates from the video exporter
	ListenerManager vexpExportUpdateListeners;
	/// Listeners for the event when the state of the video export process changes (start, completes, gets cancelled, ...)
	ListenerManager vexpExportStateChangeListeners;
	/// Validators of the video export dialog that must be satisfied to allow the start of the video export (provided that no export operation is currently running)
	ValidatorManager vexpEntryFieldValidators;

	/// Simulation time that is currently displayed
	double time;
	/// The most current estimate of the total time computed
	double computedTime;
	/// True iff the simulation has crashed
	bool crashed;

	/// Current simulation time in the video export preview playback
	double videoExportTime;
	/// Start time currently set in video export
	double videoExportStartTime;
	/// End time currently set in video export
	double videoExportEndTime;
	/// Current speed of video export preview playback
	double videoExportPlaybackSpeedup;
	/// True iff the currently set video export start and end time represent a valid range for video export (i.e. start < end)
	bool videoExportRangeValid;
	/// True iff the time in the video export preview playback time scale is currently being edited by the user
	bool videoExportEditingTime;
	/// True iff the preview playback in the video export is currently paused
	bool videoExportPlaybackPaused;
	/// Currently set filename of the video export file
	str videoExportFileLocation;
	/// Currently set width of the exported video in the video export dialog
	uint32_t videoExportWidth;
	/// Currently set height of the exported video in the video export dialog
	uint32_t videoExportHeight;
	/// Currently set bitrate of the exported video in the video export dialog
	uint32_t videoExportBitrate;

	/// Currently selected background visual mode
	uint32_t backDisplayMode;
	/// Currently selected foreground visual mode
	uint32_t frontDisplayMode;

	/// True iff the simulation playback time is currently being edited by the user
	bool editingTime;

	/// True iff the simulation playback is currently paused
	bool playbackPaused;

	/// Currently selected simulation playback mode
	PlaybackMode playbackMode;

	/// Current speed of the simulation playback
	double playbackSpeedup;

	/// True iff the video export dialog is currently open
	bool inVideoExport;
	/// The running (or completed) video exporter that exports (or has exported) the video or null if no video is being or has been exported
	uptr<VideoExporter> videoExporter;

	/**
	 * Goes back to the configuration state.
	 * Ends the current simulation, closes the video export dialog if opened.
	 */
	virtual void goBackToConfig() = 0;
	/**
	 * Pauses the computation of the simulation.
	 * Does nothing if the computation is already paused.
	 * Blocks the calling thread by joining the computation thread
	 * (but the computation thread can, when requested, terminate relatively quickly (even in the middle of the computation of one frame))
	 */
	virtual void pauseComputation() = 0;
	/**
	 * Resumes the computation of the simulation by restarting the computation thread.
	 * Does nothing if the computation (i.e. the computation thread) is already running.
	 */
	virtual void resumeComputation() = 0;

	/**
	 * Closes all windows except for the main simulation window.
	 * Meant to be called the the simulation window is getting directly closed by the user (which means the application should terminate).
	 */
	virtual void closeAll() = 0;

	/**
	 * Opens the video export dialog.
	 * Should not be called when the video export dialog is already opened.
	 */
	virtual void enterVideoExport() = 0;
	/**
	 * Closes the video export dialog.
	 * Cancels the current incomplete video export operation (if any)
	 * Should not be called when the video export dialog in not opened.
	 */
	virtual void leaveVideoExport() = 0;
	/**
	 * Starts the video export operation according to the currently set parameters.
	 * Should not be called if a video export operation is already running.
	 */
	virtual void confirmVideoExport() = 0;
	/**
	 * Cancels the current video export operation (if there is any running) and sets the video exporter to null.
	 * Does nothing if the video exporter is already null.
	 */
	virtual void cancelVideoExport() = 0;

	/**
	 * A thread-safe method for querying the state of the simulation computation
	 * @return true iff the simulation computation is completely running
	 */
	virtual bool isComputing() = 0;
	/**
	 * A thread-safe method for retrieving the number of stored computed frames
	 * @return number of stored computed frames
	 */
	virtual uint32_t getFramesStored() = 0;
	/**
	 * A thread-safe method for retrieving the number of computed iterations in the current frame
	 * @return number of computed iterations in the frame that is being computed now
	 */
	virtual uint32_t getComputedIter() = 0;

	/**
	 * Checks the validity of the specified video export time range
	 */
	virtual void videoExportValidateRange() = 0;
	/**
	 * Clamps the video export preview playback time to the interval between the set start and end times
	 */
	virtual void videoExportClampTime() = 0;

	/**
	 * Constructs the abstract simulation state by setting the app pointer
	 * @param app pointer to the parent application
	 */
	SimulationStateAbstr(ApplicationAbstr *app);
};

}

#endif // SIMULATION_STATE_ABSTR_HPP