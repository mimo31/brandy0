/**
 * export-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/2/20
 */
#include "export-window.hpp"

#include "styles.hpp"

namespace brandy0
{

ExportWindow::ExportWindow(SimulationStateAbstr *const parent)
	: parent(parent),
	selectFileButton("select file location"),
	backButton("back (no export)"),
	exportButton("export video")
{
	mainGrid.attach(startTimeLabel, 0, 0);
	mainGrid.attach(startTimeScale, 0, 1);
	mainGrid.attach(endTimeLabel, 0, 2);
	mainGrid.attach(endTimeScale, 0, 3);
	mainGrid.attach(invalidTimesLabel, 0, 4);
	mainGrid.attach(durationLabel, 0, 5);
	mainGrid.attach(playbackSpeedLabel, 0, 6);
	mainGrid.attach(playbackSpeedScale, 0, 7);
	mainGrid.attach(backButton, 0, 8);
	mainGrid.attach(timeLabel, 1, 0);
	mainGrid.attach(timeScale, 1, 1);
	mainGrid.attach(playPauseButton, 1, 2);
	mainGrid.attach(fileLocationLabel, 1, 6);
	mainGrid.attach(selectFileButton, 1, 7);
	mainGrid.attach(exportButton, 1, 8);

	add(mainGrid);

	startTimeScale.set_draw_value(false);
	startTimeScale.set_range(0, 1);

	endTimeScale.set_draw_value(false);
	endTimeScale.set_range(0, 1);

	playbackSpeedScale.set_draw_value(false);
	playbackSpeedScale.set_range(-1, 1);
	playbackSpeedScale.set_has_origin(false);

	timeScale.set_draw_value(false);
	timeScale.set_range(0, 1);

	show_all_children();

	Styles::requestInit();

	invalidTimesLabel.set_text("need start < end !");
	invalidTimesLabel.get_style_context()->add_provider(Styles::redStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	backButton.signal_clicked().connect([this, parent]
	{
		parent->leaveVideoExport();
	});

	parent->videoExportEnterListeners.plug([this]
	{
		init();
	});

	startTimeScale.signal_value_changed().connect([this, parent]
	{
		parent->videoExportStartTime = startTimeScale.get_value() * parent->computedTime;
		parent->vexpStartTimeChangeListeners.invoke();
	});

	endTimeScale.signal_value_changed().connect([this, parent]
	{
		parent->videoExportEndTime = endTimeScale.get_value() * parent->computedTime;
		parent->vexpEndTimeChangeListeners.invoke();
	});

	parent->vexpStartTimeChangeListeners.plug([this, parent]
	{
		updateStartTimeLabel();
		updateInvalidTimesWarn();
	});

	parent->vexpEndTimeChangeListeners.plug([this, parent]
	{
		updateEndTimeLabel();
		updateInvalidTimesWarn();
	});

	playbackSpeedScale.signal_value_changed().connect([this, parent]
	{
		const double scaleval = playbackSpeedScale.get_value();
		const double mult = exp(log(parent->MAX_PLAYBACK_SPEEDUP) * scaleval);
		parent->videoExportPlaybackSpeedup = mult;
		parent->vexpPlaybackSpeedupChangeListeners.invoke();
	});

	parent->vexpPlaybackSpeedupChangeListeners.plug([this, parent]
	{
		playbackSpeedLabel.set_text("playback speed " + std::to_string(parent->videoExportPlaybackSpeedup) + "x");
	});
}

void ExportWindow::updateStartTimeLabel()
{
	startTimeLabel.set_text("start = " + std::to_string(parent->videoExportStartTime) + " (of " + std::to_string(parent->computedTime) + ")");
}

void ExportWindow::updateEndTimeLabel()
{
	endTimeLabel.set_text("end = " + std::to_string(parent->videoExportEndTime) + " (of " + std::to_string(parent->computedTime) + ")");
}

void ExportWindow::updateInvalidTimesWarn()
{
	if (parent->videoExportStartTime >= parent->videoExportEndTime)
		invalidTimesLabel.pseudoShow();
	else
		invalidTimesLabel.pseudoHide();
}

void ExportWindow::updatePlaybackSpeedLabel()
{

}

void ExportWindow::init()
{
	startTimeScale.set_value(0);
	endTimeScale.set_value(1);
	playbackSpeedScale.set_value(0);
	updateStartTimeLabel();
	updateEndTimeLabel();
	updatePlaybackSpeedLabel();
}

}