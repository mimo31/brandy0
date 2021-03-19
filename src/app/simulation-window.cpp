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

	mainGrid.attach(dArea, 0, 0, 1, 1);

	playbackModeSelector.append("play until end");
	playbackModeSelector.append("loop");
	playbackModeSelector.append("last frame only");
	playbackModeSelector.set_active(0);
	playbackModeSelector.signal_changed().connect([this, parent]
	{
		const uint32_t rownumber = playbackModeSelector.get_active_row_number();
		PlaybackMode mode;
		if (rownumber == 0)
			mode = PlaybackMode::PLAY_UNTIL_END;
		else if (rownumber == 1)
			mode = PlaybackMode::LOOP;
		else
			mode = PlaybackMode::LAST_FRAME_ONLY;
		if (parent->playbackMode != mode)
		{
			parent->playbackMode = mode;
			parent->playbackModeChangeListeners.invoke();
		}
	});

	for (uint32_t i = 0; i < BACK_DISPLAY_MODE_COUNT; i++)
		backDisplaySelector.append(BACK_DISPLAY_MODES[i].name);
	backDisplaySelector.set_active(BACK_DISPLAY_MODE_DEFAULT);
	backDisplaySelector.signal_changed().connect([this, parent]
	{
		parent->backDisplayMode = backDisplaySelector.get_active_row_number();
	});

	for (uint32_t i = 0; i < FRONT_DISPLAY_MODE_COUNT; i++)
		frontDisplaySelector.append(FRONT_DISPLAY_MODES[i].name);
	frontDisplaySelector.set_active(FRONT_DISPLAY_MODE_DEFAULT);
	frontDisplaySelector.signal_changed().connect([this, parent]
	{
		parent->frontDisplayMode = frontDisplaySelector.get_active_row_number();
	});

	viewGrid.attach(backDisplayLabel, 0, 0);
	viewGrid.attach(backDisplaySelector, 0, 1);
	viewGrid.attach(frontDisplayLabel, 0, 2);
	viewGrid.attach(frontDisplaySelector, 0, 3);
	viewFrame.add(viewGrid);

	toConfigButton.signal_clicked().connect([this, parent]
	{
		parent->goBackToConfig();
	});

	timeScale.set_draw_value(false);
	timeScale.set_range(0, 1);
	timeScale.set_increments(.001, .001);
	timeScale.set_value(0);
	timeScale.signal_button_release_event().connect([parent](GdkEventButton*)
	{
		parent->editingTime = false;
		return false;
	});
	timeScale.signal_value_changed().connect([this, parent]
	{
		if (!timeScaleAutoSet)
		{
			const double scaleValue = timeScale.get_value();
			parent->time = scaleValue * parent->computedTime;
			if (!parent->playbackPaused && parent->playbackMode == PlaybackMode::LAST_FRAME_ONLY)
			{
				parent->playbackMode = PlaybackMode::PLAY_UNTIL_END;
				parent->playbackModeChangeListeners.invoke();
			}
			parent->editingTime = true;
		}
	});

	playbackSpeedScale.set_draw_value(false);
	playbackSpeedScale.set_range(-1, 1);
	playbackSpeedScale.set_increments(.001, .001);
	playbackSpeedScale.set_has_origin(false);
	playbackSpeedScale.signal_value_changed().connect([this, parent]
	{
		const double scaleval = playbackSpeedScale.get_value();
		const double mult = exp(log(parent->MAX_PLAYBACK_SPEEDUP) * scaleval);
		parent->playbackSpeedup = mult;
	});

	playbackGrid.attach(playbackModeSelector, 0, 0, 1, 2);
	playbackGrid.attach(playPauseButton, 0, 2, 1, 2);
	playbackGrid.attach(timeLabel, 1, 0);
	playbackGrid.attach(timeScale, 1, 1);
	playbackGrid.attach(playbackSpeedLabel, 1, 2);
	playbackGrid.attach(playbackSpeedScale, 1, 3);
	playbackFrame.add(playbackGrid);

	computingGrid.attach(computingSwitch, 0, 0);
	computingGrid.attach(computingStatusLabel, 0, 1);
	computingGrid.attach(frameBufferLabel, 0, 2);
	computingGrid.attach(curIterLabel, 0, 3);
	computingFrame.add(computingGrid);

	panelGrid.attach(toConfigButton, 0, 0);
	panelGrid.attach(computingFrame, 1, 0);
	panelGrid.attach(playbackFrame, 2, 0);
	panelGrid.attach(viewFrame, 3, 0);
	panelGrid.attach(videoExportButton, 4, 0);

	mainGrid.attach(panelGrid, 0, 1);

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
		backDisplaySelector.set_active(BACK_DISPLAY_MODE_DEFAULT);
		frontDisplaySelector.set_active(FRONT_DISPLAY_MODE_DEFAULT);
		computingSwitch.set_sensitive(true);
		timeScaleAutoSet = true;
		timeScale.set_value(0);
		timeScaleAutoSet = false;
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
	if (parent->playbackMode == PlaybackMode::PLAY_UNTIL_END)
		actrow = 0;
	else if (parent->playbackMode == PlaybackMode::LOOP)
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
		timeScaleAutoSet = true;
		timeScale.set_value(val);
		timeScaleAutoSet = false;
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
	computingGrid.remove(computingSwitch);
}

}
