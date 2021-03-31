/**
 * simulation-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#include "simulation-window.hpp"

#include "conv-utils.hpp"
#include "display-modes.hpp"

namespace brandy0
{

const str exportButtonDefaultLabel = "export video";

SimulationWindow::SimulationWindow(SimulationStateAbstr *const parent)
	: BrandyWindow(1280, 720),
	parent(parent), dArea(parent),
	toConfigButton("back to config"),
	computingFrame("computation"),
	playbackFrame("playback"),
	playPauseButton("pause"),
	viewFrame("visuals"),
	backDisplayLabel("background visual"), frontDisplayLabel("foreground visual"),
	videoExportButton(exportButtonDefaultLabel)
{
	timeLabel.set_size_request(300, -1);
	frameBufferLabel.set_size_request(200, -1);

	mainGrid.set_orientation(Gtk::ORIENTATION_VERTICAL);

	mainGrid.add(dArea);

	mainGrid.set_row_spacing(StyleManager::WidgetSpacing);

	playbackModeSelector.append("play until end");
	playbackModeSelector.append("loop");
	playbackModeSelector.append("last frame only");
	playbackModeSelector.set_active(0);
	playbackModeSelector.signal_changed().connect([this, parent]
	{
		const uint32_t rownumber = playbackModeSelector.get_active_row_number();
		PlaybackMode mode;
		if (rownumber == 0)
			mode = PlaybackMode::PlayUntilEnd;
		else if (rownumber == 1)
			mode = PlaybackMode::Loop;
		else
			mode = PlaybackMode::LastFrameOnly;
		if (parent->playbackMode != mode)
		{
			parent->playbackMode = mode;
			parent->playbackModeChangeListeners.invoke();
		}
	});

	for (const BackDisplayMode mode : BackDisplayModes)
		backDisplaySelector.append(mode.name);
	backDisplaySelector.set_active(BackDisplayModeDefault);
	backDisplaySelector.signal_changed().connect([this, parent]
	{
		parent->backDisplayMode = backDisplaySelector.get_active_row_number();
	});

	for (const FrontDisplayMode mode : FrontDisplayModes)
		frontDisplaySelector.append(mode.name);
	frontDisplaySelector.set_active(FrontDisplayModeDefault);
	frontDisplaySelector.signal_changed().connect([this, parent]
	{
		parent->frontDisplayMode = frontDisplaySelector.get_active_row_number();
	});

	viewGrid.attach(backDisplayLabel, 0, 0);
	viewGrid.attach(backDisplaySelector, 0, 1);
	viewGrid.attach(frontDisplayLabel, 0, 2);
	viewGrid.attach(frontDisplaySelector, 0, 3);
	StyleManager::setPadding(viewGrid);
	viewFrame.add(viewGrid);

	toConfigButton.signal_clicked().connect([this, parent]
	{
		parent->goBackToConfig();
	});

	timeScale.set_value(0);
	timeScale.signal_button_release_event().connect([parent](GdkEventButton*)
	{
		parent->editingTime = false;
		return false;
	});
	timeScale.connectUserChangedHandler([this, parent]
	{
		const double scaleValue = timeScale.get_value();
		parent->time = scaleValue * parent->computedTime;
		if (!parent->playbackPaused && parent->playbackMode == PlaybackMode::LastFrameOnly)
		{
			parent->playbackMode = PlaybackMode::PlayUntilEnd;
			parent->playbackModeChangeListeners.invoke();
		}
		parent->editingTime = true;
	});

	playbackSpeedScale.signal_value_changed().connect([this, parent]
	{
		parent->playbackSpeedup = playbackSpeedScale.getSpeedup();
	});

	playbackGrid.attach(playbackModeSelector, 0, 0, 1, 2);
	playbackGrid.attach(playPauseButton, 0, 2, 1, 2);
	playbackGrid.attach(timeLabel, 1, 0);
	playbackGrid.attach(timeScale, 1, 1);
	playbackGrid.attach(playbackSpeedLabel, 1, 2);
	playbackGrid.attach(playbackSpeedScale, 1, 3);
	StyleManager::setPadding(playbackGrid);
	playbackFrame.add(playbackGrid);

	computingGrid.attach(computingSwitch, 0, 0);
	computingGrid.attach(computingStatusLabel, 0, 1);
	computingGrid.attach(frameBufferLabel, 0, 2);
	computingGrid.attach(curIterLabel, 0, 3);
	StyleManager::setPadding(computingGrid);
	computingFrame.add(computingGrid);

	panelGrid.attach(toConfigButton, 0, 0);
	panelGrid.attach(computingFrame, 1, 0);
	panelGrid.attach(playbackFrame, 2, 0);
	panelGrid.attach(viewFrame, 3, 0);
	panelGrid.attach(videoExportButton, 4, 0);

	panelGrid.set_column_spacing(StyleManager::WidgetSpacing);

	mainGrid.add(panelGrid);

	add(mainGrid);

	show_all_children();

	const str divergedMessage = "SIMULATION DIVERGED";

	parent->computingSwitchListeners.plug([this, parent, divergedMessage]{
		const bool computing = parent->isComputing();
		computingStatusLabel.set_text(!parent->crashed ? (computing ? "running" : "paused") : divergedMessage);
		if (computingSwitch.get_state() != computing)
			computingSwitch.set_state(computing);
	});

	computingSwitch.property_state().signal_changed().connect([this, parent]{
		if (computingSwitch.get_state())
			parent->resumeComputation();
		else
			parent->pauseComputation();
	});

	parent->crashListeners.plug([this, divergedMessage]
	{
		computingSwitch.set_state(false);
		computingSwitch.set_sensitive(false);
		computingStatusLabel.set_text(divergedMessage);
	});

	parent->initListeners.plug([this, parent]{
		playPauseButton.set_label("pause");
		updatePlaybackModeSelector();
		backDisplaySelector.set_active(BackDisplayModeDefault);
		frontDisplaySelector.set_active(FrontDisplayModeDefault);
		computingSwitch.set_sensitive(true);
		timeScale.quietSetValue(0);
		playbackSpeedScale.set_value(0);
	});

	playPauseButton.signal_clicked().connect([this, parent]{
		parent->playbackPaused = !parent->playbackPaused;
		parent->playbackStateChangeListeners.invoke();
	});

	parent->playbackStateChangeListeners.plug([this, parent]{
		playPauseButton.set_label(parent->playbackPaused ? "resume" : "pause");
	});

	parent->playbackModeChangeListeners.plug([this]{
		updatePlaybackModeSelector();
	});

	videoExportButton.signal_clicked().connect([this, parent]
	{
		parent->enterVideoExport();
	});

	parent->updateListeners.plug([this]
	{
		update();
	});

	signal_delete_event().connect([parent](GdkEventAny*)
	{
		parent->closeAll();
		return false;
	});

	parent->vexpEnterListeners.plug([this]
	{
		disableWhenExport();
	});

	parent->vexpLeaveListeners.plug([this]
	{
		enableWhenNotExport();
	});
}

void SimulationWindow::updatePlaybackModeSelector()
{
	uint32_t actrow;
	if (parent->playbackMode == PlaybackMode::PlayUntilEnd)
		actrow = 0;
	else if (parent->playbackMode == PlaybackMode::Loop)
		actrow = 1;
	else
		actrow = 2;
	playbackModeSelector.set_active(actrow);
}

void SimulationWindow::update()
{
	if (!parent->inVideoExport)
		updateStats();
	dArea.redraw();
}

void SimulationWindow::updateStats()
{
	if (!parent->editingTime)
	{
		const double val = parent->computedTime != 0 ? parent->time / parent->computedTime : 0;
		timeScale.quietSetValue(val);
	}
	else
	{
		parent->time = parent->computedTime * timeScale.get_value();	
	}
	const uint32_t fcap = parent->params->frameCapacity;
	frameBufferLabel.set_text("frames in buffer: " + ConvUtils::intToZeropadStringByOrder(parent->getFramesStored(), fcap) + " / " + std::to_string(fcap));
	const uint32_t sperframe = parent->params->stepsPerFrame;
	curIterLabel.set_text("iter. of frame: " + ConvUtils::intToZeropadStringByOrder(parent->getComputedIter(), sperframe) + " / " + std::to_string(sperframe));
	timeLabel.set_text("t = " + ConvUtils::timeToString(parent->time, parent->computedTime) + " (of " + ConvUtils::timeToString(parent->computedTime) + ")");
	playbackSpeedLabel.set_text("playback speed " + ConvUtils::speedupToString(parent->playbackSpeedup) + "x");
}

void SimulationWindow::disableWhenExport()
{
	computingSwitch.set_sensitive(false);
	playbackModeSelector.set_sensitive(false);
	playPauseButton.set_sensitive(false);
	timeScale.set_sensitive(false);
	playbackSpeedScale.set_sensitive(false);
	videoExportButton.set_sensitive(false);
	videoExportButton.set_label("(exporting)");
}

void SimulationWindow::enableWhenNotExport()
{
	computingSwitch.set_sensitive(true);
	playbackModeSelector.set_sensitive(true);
	playPauseButton.set_sensitive(true);
	timeScale.set_sensitive(true);
	playbackSpeedScale.set_sensitive(true);
	videoExportButton.set_sensitive(true);
	videoExportButton.set_label(exportButtonDefaultLabel);
}

SimulationWindow::~SimulationWindow()
{
	// prevent segfault due to a GTK+ 3 bug (see https://gitlab.gnome.org/GNOME/gtk/-/issues/3720)
	computingGrid.remove(computingSwitch);
}

}
