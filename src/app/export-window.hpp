/**
 * export-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#ifndef EXPORT_WINDOW_HPP
#define EXPORT_WINDOW_HPP

#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scale.h>

#include "annotated-entry.hpp"
#include "brandy-window.hpp"
#include "hideable.hpp"
#include "scales.hpp"
#include "simulation-state-abstr.hpp"

namespace brandy0
{

/**
 * Class representing the window for exporting videos of the visualized simulation data
 */
class ExportWindow : public BrandyWindow
{
private:
	/// Pointer to the parent (abstract) simulation state
	SimulationStateAbstr *parent;

	/// TimeScale to set the start time of the exported video
	TimeScale startTimeScale;
	/// TimeScale to set the end time of the exported video
	TimeScale endTimeScale;
	/// TimeScale for the current time of the video preview
	MutableTimeScale timeScale;
	/// SpeedScale for the preview playback speedup
	SpeedScale playbackSpeedScale;
	/// Label displaying the video export start time
	Gtk::Label startTimeLabel;
	/// Label displaying the video export end time
	Gtk::Label endTimeLabel;
	/// Label displaying the preview playback speedup
	Gtk::Label playbackSpeedLabel;
	/// Label displaying the current time of the preview playback
	Gtk::Label timeLabel;
	/// Label displaying the selected file for saving the exported video
	Gtk::Label fileLocationLabel;
	/// Label with a message that the start and end times are invalid (because !(start < end))
	Hideable<Gtk::Label> invalidTimesLabel;
	/// Label displaying the estimated duration of the exported video
	Hideable<Gtk::Label> durationLabel;
	/// Label displaying the video export progress (as a number of frames)
	Hideable<Gtk::Label> exportProgressLabel;
	/// Button for playing/pausing/resuming the preview playback
	Gtk::Button playPauseButton;
	/// Button for opening the save file selection dialog
	Gtk::Button selectFileButton;
	/// Button for leaving (closing) the export window
	Gtk::Button backButton;
	/// Button for starting the video export
	Gtk::Button exportButton;
	/// Window's main grid
	Gtk::Grid mainGrid;
	/// Grid with the widgets for setting the parameters of the exported video realated to time (start, end, speed)
	Gtk::Grid timingGrid;
	/// Grid with the widgets for controlling the preview playback
	Gtk::Grid previewGrid;
	/// Grid with the widgets for setting the video file parameters (resolution, bitrate, filename)
	Gtk::Grid parameterGrid;
	/// Progress bar with displaying the progress of video export
	Hideable<Gtk::ProgressBar> exportProgressBar;
	/// Entry for the width of the exported video (in pixels)
	AnnotatedEntry widthEntry;
	/// Entry for the height of the exported video (in pixels)
	AnnotatedEntry heightEntry;
	/// Entry for the bitrate of the exported video
	AnnotatedEntry bitrateEntry;
	/// Frame for @see timingGrid
	Gtk::Frame timingFrame;
	/// Frame for @see previewGrid
	Gtk::Frame previewFrame;
	/// Frame for @see parameterGrid
	Gtk::Frame parameterFrame;

	/**
	 * Initializes the window when it gets shown
	 */
	void init();
	/**
	 * Sets the text in the start time label based on data stored in the parent simulation state
	 */
	void updateStartTimeLabel();
	/**
	 * Sets the text in the end time label based on data stored in the parent simulation state
	 */
	void updateEndTimeLabel();
	/**
	 * Sets the (pseudo)visibility of the invalid times warning label based on parent->videoExportRangeValid
	 */
	void updateInvalidTimesWarn();
	/**
	 * Sets the text in the playback speed label based on data stored in the parent simulation state
	 */
	void updatePlaybackSpeedLabel();
	/**
	 * Sets the text in and the (pseudo)visibility of the duration label based on data stored in the parent simulation state
	 */
	void updateDurationLabel();
	/**
	 * Sets the text in the preview playback time label based on data stored in the parent state
	 */
	void updateTimeLabel();
	/**
	 * Sets the label on the play/pause/resume button based on data stored in the parent state
	 */
	void updatePlayPauseButtonLabel();
	/**
	 * Sets the sensitivity of the export button based on data stored in the parent state (based on whether all input is valid)
	 */
	void updateExportButtonSensitivity();
	/**
	 * Sets the visibility and the state (the content) of the export progress bar and the related progress label based on data stored in the parent state
	 */
	void updateProgressIndicators();
	/**
	 * Sets the text in the label with the selected filename for the exported video
	 */
	void updateFileLocationLabel();
	/**
	 * Sets the label on export button (to either something like "export" or something like "cancel") based on data in the parent state
	 */
	void updateExportButtonLabel();

	/**
	 * Sets the exported video filename in the parent state by showing a file dialog to the user and letting them select.
	 * This method blocks execution on the calling thread until the user selects a file (or otherwise exits the dialog)
	 */
	void getFileLocationFromUser();

	/**
	 * Sets the value of the video preview time scale directly based on data about the start, the end, and the current times in the parent state
	 */
	void setTimeScaleFromTime();

	/**
	 * Updates the parts of the window that might continuously change (in particular, those that depend on the current video preview time)
	 */
	void update();
	
	/**
	 * Connects appropriate handlers to signals and events produced by this window and its widgets
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectWindowEventHandlers();
	/**
	 * Connects appropriate handlers to events managed by the parent state
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectStateEventHandlers();

public:
	/**
	 * Constructs the export window object
	 * @param parent pointer to the parent state
	 */
	ExportWindow(SimulationStateAbstr *parent);
};

}

#endif // EXPORT_WINDOW_HPP