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
	mainGrid.attach(exportProgressLabel, 0, 9, 2, 1);
	mainGrid.attach(exportProgressBar, 0, 10, 2, 1);

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
		parent->videoExportClampTime();
		parent->videoExportValidateRange();
		parent->vexpStartTimeChangeListeners.invoke();
	});

	endTimeScale.signal_value_changed().connect([this, parent]
	{
		parent->videoExportEndTime = endTimeScale.get_value() * parent->computedTime;
		parent->videoExportClampTime();
		parent->videoExportValidateRange();
		parent->vexpEndTimeChangeListeners.invoke();
	});

	parent->vexpStartTimeChangeListeners.plug([this]
	{
		updateStartTimeLabel();
		updateDurationLabel();
	});

	parent->vexpEndTimeChangeListeners.plug([this]
	{
		updateEndTimeLabel();
		updateDurationLabel();
	});

	playbackSpeedScale.signal_value_changed().connect([this, parent]
	{
		const double scaleval = playbackSpeedScale.get_value();
		const double mult = exp(log(parent->MAX_PLAYBACK_SPEEDUP) * scaleval);
		parent->videoExportPlaybackSpeedup = mult;
		parent->vexpPlaybackSpeedupChangeListeners.invoke();
	});

	parent->vexpPlaybackSpeedupChangeListeners.plug([this]
	{
		updatePlaybackSpeedLabel();
		updateDurationLabel();
	});

	timeScale.signal_value_changed().connect([this, parent]
	{
		if (!timeScaleAutoSet)
		{
			parent->videoExportEditingTime = true;
			if (parent->videoExportRangeValid)
			{
				parent->videoExportTime = parent->videoExportStartTime + timeScale.get_value() * (parent->videoExportEndTime - parent->videoExportStartTime);
			}
		}
	});
	timeScale.signal_button_release_event().connect([this, parent](GdkEventButton*)
	{
		parent->videoExportEditingTime = false;
		return false;
	});

	parent->updateListeners.plug([this, parent]
	{
		if (parent->inVideoExport)
			update();
	});

	parent->vexpRangeValidityChangeListeners.plug([this, parent]
	{
		updateInvalidTimesWarn();
		updateExportButtonSensitivity();
	});

	parent->vexpPlaybackStateChangeListeners.plug([this]
	{
		updatePlayPauseButtonLabel();
	});

	playPauseButton.signal_clicked().connect([this, parent]
	{
		parent->videoExportPlaybackPaused = !parent->videoExportPlaybackPaused;
		parent->vexpPlaybackStateChangeListeners.invoke();
	});

	exportButton.signal_clicked().connect([this, parent]
	{
		parent->confirmVideoExport();
	});

	parent->vexpExportUpdateListeners.plug([this]
	{
		updateProgressIndicators();
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
	if (parent->videoExportRangeValid)
		invalidTimesLabel.pseudoHide();
	else
		invalidTimesLabel.pseudoShow();
}

void ExportWindow::updatePlaybackSpeedLabel()
{
	playbackSpeedLabel.set_text("playback speed " + std::to_string(parent->videoExportPlaybackSpeedup) + "x");
}

void ExportWindow::updateDurationLabel()
{
	if (parent->videoExportRangeValid)
	{
		const double simtime = parent->videoExportEndTime - parent->videoExportStartTime;
		const double realtime = simtime / (parent->params->dt * parent->params->stepsPerFrame * parent->videoExportPlaybackSpeedup) * parent->MS_PER_BASE_FRAME / 1000;
		durationLabel.set_text("duration = " + std::to_string(simtime) + " (" + std::to_string(realtime) + " s)");
		durationLabel.pseudoShow();
	}
	else
	{
		durationLabel.set_text("duration");
		durationLabel.pseudoHide();
	}
}

void ExportWindow::updateTimeLabel()
{
	if (parent->videoExportRangeValid)
	{
		const double range = parent->videoExportEndTime - parent->videoExportStartTime;
		const double tm = parent->videoExportTime;
		const double frac = (tm - parent->videoExportStartTime) / range;
		timeLabel.set_text("t = " + std::to_string(parent->videoExportTime) + " (" + std::to_string(100 * frac) + " %)");
	}
	else
	{
		timeLabel.set_text("t undefined");
	}
}

void ExportWindow::updatePlayPauseButtonLabel()
{
	playPauseButton.set_label(parent->videoExportPlaybackPaused ? "play" : "pause");
}

void ExportWindow::updateExportButtonSensitivity()
{
	exportButton.set_sensitive(parent->videoExportRangeValid);
}

void ExportWindow::setTimeScale(const double scaleVal)
{
	timeScaleAutoSet = true;
	timeScale.set_value(scaleVal);
	timeScaleAutoSet = false;
}

void ExportWindow::setTimeScaleFromTime()
{
	if (parent->videoExportRangeValid)
	{
		const double fromStart = parent->videoExportTime - parent->videoExportStartTime;
		const double range = parent->videoExportEndTime - parent->videoExportStartTime;
		setTimeScale(fromStart / range);
	}
}

void ExportWindow::updateProgressIndicators()
{
	if (parent->videoExporter)
	{
		exportProgressBar.pseudoShow();
		exportProgressLabel.pseudoShow();
		if (parent->videoExporter->complete)
		{
			exportProgressBar.set_fraction(1);
			exportProgressLabel.set_text("export complete!");
		}
		else if (parent->videoExporter->finishing)
		{
			exportProgressBar.set_fraction(1);
			const std::string num = std::to_string(parent->videoExporter->framesToProcess);
			exportProgressLabel.set_text("processed " + num + " / " + num + " video frames, finishing...");
		}
		else if (parent->videoExporter->failed)
		{
			exportProgressLabel.set_text("export failed (" + parent->videoExporter->errorMessage + ")");
		}
		else
		{
			exportProgressBar.set_fraction(parent->videoExporter->processedFrames / double(parent->videoExporter->framesToProcess));
			const std::string processingFr = std::to_string(parent->videoExporter->processedFrames + 1),
				totalFr = std::to_string(parent->videoExporter->framesToProcess);
			exportProgressLabel.set_text("processing video frame " + processingFr + " / " + totalFr + "...");
		}
	}
	else
	{
		exportProgressBar.pseudoHide();
		exportProgressLabel.pseudoHide();
	}
}

void ExportWindow::update()
{
	updateTimeLabel();
	if (!parent->videoExportEditingTime)
	{
		setTimeScaleFromTime();
	}
}

void ExportWindow::init()
{
	startTimeScale.set_value(0);
	endTimeScale.set_value(1);
	playbackSpeedScale.set_value(0);
	timeScaleAutoSet = false;
	updateStartTimeLabel();
	updateEndTimeLabel();
	updatePlaybackSpeedLabel();
	updateInvalidTimesWarn();
	updatePlayPauseButtonLabel();
	updateDurationLabel();
	updateExportButtonSensitivity();
	updateProgressIndicators();
}

}