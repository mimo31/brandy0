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

class ExportWindow : public BrandyWindow
{
private:
	SimulationStateAbstr *parent;

	TimeScale startTimeScale, endTimeScale, timeScale;
	SpeedScale playbackSpeedScale;
	Gtk::Label startTimeLabel, endTimeLabel, playbackSpeedLabel, timeLabel, fileLocationLabel;
	Hideable<Gtk::Label> invalidTimesLabel, durationLabel, exportProgressLabel;
	Gtk::Button playPauseButton, selectFileButton, backButton, exportButton;
	Gtk::Grid mainGrid, timingGrid, previewGrid, parameterGrid;
	Hideable<Gtk::ProgressBar> exportProgressBar;
	AnnotatedEntry widthEntry, heightEntry, bitrateEntry;
	Gtk::Frame timingFrame, previewFrame, parameterFrame;

	bool timeScaleAutoSet;

	void init();
	void updateStartTimeLabel();
	void updateEndTimeLabel();
	void updateInvalidTimesWarn();
	void updatePlaybackSpeedLabel();
	void updateDurationLabel();
	void updateTimeLabel();
	void updatePlayPauseButtonLabel();
	void updateExportButtonSensitivity();
	void updateProgressIndicators();
	void updateFileLocationLabel();

	void getFileLocationFromUser();

	void setTimeScale(double scaleVal);
	void setTimeScaleFromTime();

	void update();
	
	void connectWindowEventHandlers();
	void connectStateEventHandlers();

public:
	ExportWindow(SimulationStateAbstr *parent);
};

}

#endif // EXPORT_WINDOW_HPP