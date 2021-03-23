/**
 * export-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#include "export-window.hpp"

#include <gtkmm/filechooserdialog.h>

#include "conv-utils.hpp"
#include "style-manager.hpp"
#include "str.hpp"

namespace brandy0
{

ExportWindow::ExportWindow(SimulationStateAbstr *const parent)
	: parent(parent),
	selectFileButton("select file location"),
	backButton("cancel"),
	exportButton("export video"),
	widthEntry("width (pixels):", parent->app->styleManager),
	heightEntry("height (pixels):", parent->app->styleManager),
	bitrateEntry("bitrate:", parent->app->styleManager),
	timingFrame("video timing"),
	previewFrame("preview play"),
	parameterFrame("video file parameters")
{
	startTimeLabel.set_size_request(300, -1);
	timeLabel.set_size_request(200, -1);

	timingGrid.attach(startTimeLabel, 0, 0);
	timingGrid.attach(startTimeScale, 0, 1);
	timingGrid.attach(endTimeLabel, 0, 2);
	timingGrid.attach(endTimeScale, 0, 3);
	timingGrid.attach(invalidTimesLabel, 0, 4);
	timingGrid.attach(durationLabel, 0, 5);
	timingGrid.attach(playbackSpeedLabel, 0, 6);
	timingGrid.attach(playbackSpeedScale, 0, 7);
	timingFrame.add(timingGrid);
	mainGrid.attach(timingFrame, 0, 0, 1, 2);

	previewGrid.attach(timeLabel, 0, 0);
	previewGrid.attach(timeScale, 0, 1);
	previewGrid.attach(playPauseButton, 0, 2);
	previewFrame.add(previewGrid);
	mainGrid.attach(previewFrame, 1, 0);

	widthEntry.attachTo(parameterGrid, 0, 0);
	heightEntry.attachTo(parameterGrid, 0, 1);
	bitrateEntry.attachTo(parameterGrid, 0, 2);
	parameterGrid.attach(fileLocationLabel, 0, 3, 3, 1);
	parameterGrid.attach(selectFileButton, 0, 4, 3, 1);
	parameterFrame.add(parameterGrid);
	mainGrid.attach(parameterFrame, 1, 1);

	mainGrid.attach(backButton, 0, 2);
	mainGrid.attach(exportButton, 1, 2);
	mainGrid.attach(exportProgressLabel, 0, 3, 2, 1);
	mainGrid.attach(exportProgressBar, 0, 4, 2, 1);

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

	parent->app->styleManager.requestInit();

	invalidTimesLabel.set_text("need start < end !");
	invalidTimesLabel.get_style_context()->add_provider(parent->app->styleManager.redStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	backButton.signal_clicked().connect([this, parent]
	{
		parent->leaveVideoExport();
		hide();
	});

	parent->vexpEnterListeners.plug([this]
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

	selectFileButton.signal_clicked().connect([this, parent]
	{
		getFileLocationFromUser();
	});

	parent->vexpFileLocationChangeListeners.plug([this]
	{
		updateFileLocationLabel();
	});

	widthEntry.hookInputHandler([this, parent]
	{
		uint32_t w;
		ConvUtils::updatePosIntIndicator(widthEntry, w, parent->DEFAULT_VIDEO_WIDTH, parent->MAX_VIDEO_WIDTH);
		if (widthEntry.hasValidInput() && w % 4 != 0)
			widthEntry.indicateInvalid("must be divisible by 4");
		else
			parent->videoExportWidth = w;
		parent->vexpEntryValidityChangeListeners.invoke();
	});

	heightEntry.hookInputHandler([this, parent]
	{
		uint32_t h;
		ConvUtils::updatePosIntIndicator(heightEntry, h, parent->DEFAULT_VIDEO_HEIGHT, parent->MAX_VIDEO_HEIGHT);
		if (heightEntry.hasValidInput() && h % 4 != 0)
			heightEntry.indicateInvalid("must be divisible by 4");
		else
			parent->videoExportHeight = h;
		parent->vexpEntryValidityChangeListeners.invoke();
	});

	bitrateEntry.hookInputHandler([this, parent]
	{
		ConvUtils::updatePosIntIndicator(bitrateEntry, parent->videoExportBitrate, parent->DEFAULT_VIDEO_BITRATE, parent->MAX_VIDEO_BITRATE);
		parent->vexpEntryValidityChangeListeners.invoke();
	});

	parent->entryFieldValidators.plug([this]
	{
		return widthEntry.hasValidInput() && heightEntry.hasValidInput() && bitrateEntry.hasValidInput();
	});

	parent->vexpEntryValidityChangeListeners.plug([this]
	{
		updateExportButtonSensitivity();
	});

	signal_delete_event().connect([parent](GdkEventAny*)
	{
		parent->leaveVideoExport();
		return false;
	});
}

void ExportWindow::getFileLocationFromUser()
{
	Gtk::FileChooserDialog dial("Select file for exported video.", Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SAVE);
	dial.set_transient_for(*this);
	dial.set_modal(true);
	dial.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
	dial.add_button("Select", Gtk::ResponseType::RESPONSE_OK);

	// dial.run() blocks this thread
	if (dial.run() == Gtk::ResponseType::RESPONSE_OK)
	{
		Glib::RefPtr<Gio::File> selectedFile = dial.get_file(), cd = Gio::File::create_for_path(".");
		const str relpath = cd->get_relative_path(selectedFile);
		str fname = relpath.empty() ? selectedFile->get_path() : "./" + relpath;
		if (fname.length() < 4 || fname.substr(fname.length() - 4) != ".mp4")
			fname += ".mp4";
		parent->videoExportFileLocation = fname;
		parent->vexpFileLocationChangeListeners.invoke();
	}
}

void ExportWindow::updateStartTimeLabel()
{
	startTimeLabel.set_text("start = " + ConvUtils::timeToString(parent->videoExportStartTime, parent->computedTime) + " (of " + ConvUtils::timeToString(parent->computedTime) + ")");
}

void ExportWindow::updateEndTimeLabel()
{
	endTimeLabel.set_text("end = " + ConvUtils::timeToString(parent->videoExportEndTime, parent->computedTime) + " (of " + ConvUtils::timeToString(parent->computedTime) + ")");
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
	playbackSpeedLabel.set_text("playback speed " + ConvUtils::speedupToString(parent->videoExportPlaybackSpeedup) + "x");
}

void ExportWindow::updateDurationLabel()
{
	if (parent->videoExportRangeValid)
	{
		const double simtime = parent->videoExportEndTime - parent->videoExportStartTime;
		const double realtime = simtime / (parent->params->dt * parent->params->stepsPerFrame * parent->videoExportPlaybackSpeedup) * parent->MS_PER_BASE_FRAME / 1000;
		durationLabel.set_text("duration = " + ConvUtils::timeToString(simtime) + " (" + ConvUtils::timeToString(realtime) + " s)");
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
		timeLabel.set_text("t = " + ConvUtils::timeToString(parent->videoExportTime, parent->videoExportEndTime) + " (" + ConvUtils::percentageToString(100 * frac) + " %)");
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
	exportButton.set_sensitive(parent->videoExportRangeValid && parent->entryFieldValidators.isAllValid());
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
			backButton.set_label("close");
		}
		else if (parent->videoExporter->finishing)
		{
			exportProgressBar.set_fraction(1);
			const str num = std::to_string(parent->videoExporter->framesToProcess);
			exportProgressLabel.set_text("processed " + num + " / " + num + " video frames, finishing...");
		}
		else if (parent->videoExporter->failed)
		{
			exportProgressLabel.set_text("export failed (" + parent->videoExporter->errorMessage + ")");
		}
		else
		{
			exportProgressBar.set_fraction(parent->videoExporter->processedFrames / double(parent->videoExporter->framesToProcess));
			const str processingFr = std::to_string(parent->videoExporter->processedFrames + 1);
			const str totalFr = std::to_string(parent->videoExporter->framesToProcess);
			exportProgressLabel.set_text("processing video frame " + processingFr + " / " + totalFr + "...");
		}
	}
	else
	{
		exportProgressBar.pseudoHide();
		exportProgressLabel.pseudoHide();
	}
}

void ExportWindow::updateFileLocationLabel()
{
	fileLocationLabel.set_text("will save to " + parent->videoExportFileLocation);
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

	widthEntry.setText(ConvUtils::defaultToString(parent->videoExportWidth));
	heightEntry.setText(ConvUtils::defaultToString(parent->videoExportHeight));
	bitrateEntry.setText(std::to_string(parent->videoExportBitrate));

	updateStartTimeLabel();
	updateEndTimeLabel();
	updatePlaybackSpeedLabel();
	updateInvalidTimesWarn();
	updatePlayPauseButtonLabel();
	updateDurationLabel();
	updateExportButtonSensitivity();
	updateProgressIndicators();
	updateFileLocationLabel();
}

}