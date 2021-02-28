/**
 * export-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/2/20
 */
#ifndef EXPORT_WINDOW_HPP
#define EXPORT_WINDOW_HPP

#include <gtkmm/button.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/scale.h>

#include "brandy-window.hpp"
#include "hideable.hpp"
#include "simulation-state-abstr.hpp"

namespace brandy0
{

class ExportWindow : public BrandyWindow
{
private:
	SimulationStateAbstr *parent;

	Gtk::Scale startTimeScale, endTimeScale, playbackSpeedScale, timeScale;
	Gtk::Label startTimeLabel, endTimeLabel, playbackSpeedLabel, timeLabel, fileLocationLabel;
	Hideable<Gtk::Label> invalidTimesLabel, durationLabel, exportProgressLabel;
	Gtk::Button playPauseButton, selectFileButton, backButton, exportButton;
	Gtk::Grid mainGrid;
	Hideable<Gtk::ProgressBar> exportProgressBar;

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

	void setTimeScale(const double scaleVal);
	void setTimeScaleFromTime();

	void update();

public:
	ExportWindow(SimulationStateAbstr *const parent);
};

}

#endif // EXPORT_WINDOW_HPP