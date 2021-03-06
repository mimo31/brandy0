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

const str exportButtonDefaultLabel = "export video";

ExportWindow::ExportWindow(SimulationStateAbstr *const parent)
	: parent(parent),
	selectFileButton("select file location"),
	backButton("cancel"),
	exportButton(exportButtonDefaultLabel),
	widthEntry("width (pixels):", &parent->app->styleManager),
	heightEntry("height (pixels):", &parent->app->styleManager),
	bitrateEntry("bitrate:", &parent->app->styleManager),
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
	StyleManager::setPadding(timingGrid);
	timingFrame.add(timingGrid);
	mainGrid.attach(timingFrame, 0, 0, 1, 2);

	previewGrid.attach(timeLabel, 0, 0);
	previewGrid.attach(timeScale, 0, 1);
	previewGrid.attach(playPauseButton, 0, 2);
	StyleManager::setPadding(previewGrid);
	timeLabel.set_hexpand(true);
	timeScale.set_hexpand(true);
	playPauseButton.set_hexpand(true);
	previewGrid.set_hexpand(true);
	previewFrame.add(previewGrid);
	mainGrid.attach(previewFrame, 1, 0);

	widthEntry.attachTo(parameterGrid, 0, 0);
	heightEntry.attachTo(parameterGrid, 0, 1);
	bitrateEntry.attachTo(parameterGrid, 0, 2);
	parameterGrid.attach(fileLocationLabel, 0, 3, 3, 1);
	parameterGrid.attach(selectFileButton, 0, 4, 3, 1);
	fileLocationLabel.set_hexpand(true);
	selectFileButton.set_hexpand(true);
	StyleManager::setPadding(parameterGrid);
	parameterGrid.set_column_spacing(StyleManager::WidgetSpacing);

	parameterFrame.add(parameterGrid);
	mainGrid.attach(parameterFrame, 1, 1);

	mainGrid.attach(backButton, 0, 2);
	mainGrid.attach(exportButton, 1, 2);
	mainGrid.attach(exportProgressLabel, 0, 3, 2, 1);
	mainGrid.attach(exportProgressBar, 0, 4, 2, 1);

	mainGrid.set_row_spacing(StyleManager::WidgetSpacing);
	mainGrid.set_column_spacing(StyleManager::WidgetSpacing);

	add(mainGrid);

	show_all_children();

	parent->app->styleManager.requestInit();

	invalidTimesLabel.set_text("need start < end !");
	invalidTimesLabel.get_style_context()->add_provider(parent->app->styleManager.redStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	connectWindowEventHandlers();
	connectStateEventHandlers();
}

void ExportWindow::connectWindowEventHandlers()
{
	backButton.signal_clicked().connect([this]
	{
		parent->leaveVideoExport();
		hide();
	});
	startTimeScale.signal_value_changed().connect([this]
	{
		parent->videoExportStartTime = startTimeScale.get_value() * parent->computedTime;
		parent->videoExportClampTime();
		parent->videoExportValidateRange();
		parent->vexpStartTimeChangeListeners.invoke();
	});
	endTimeScale.signal_value_changed().connect([this]
	{
		parent->videoExportEndTime = endTimeScale.get_value() * parent->computedTime;
		parent->videoExportClampTime();
		parent->videoExportValidateRange();
		parent->vexpEndTimeChangeListeners.invoke();
	});
	playbackSpeedScale.signal_value_changed().connect([this]
	{
		parent->videoExportPlaybackSpeedup = playbackSpeedScale.getSpeedup();
		parent->vexpPlaybackSpeedupChangeListeners.invoke();
	});
	timeScale.connectUserChangedHandler([this]
	{
		parent->videoExportEditingTime = true;
		if (parent->videoExportRangeValid)
		{
			parent->videoExportTime = parent->videoExportStartTime + timeScale.get_value() * (parent->videoExportEndTime - parent->videoExportStartTime);
		}
	});
	timeScale.signal_button_release_event().connect([this](GdkEventButton*)
	{
		parent->videoExportEditingTime = false;
		return false;
	});
	playPauseButton.signal_clicked().connect([this]
	{
		parent->videoExportPlaybackPaused = !parent->videoExportPlaybackPaused;
		parent->vexpPlaybackStateChangeListeners.invoke();
	});
	exportButton.signal_clicked().connect([this]
	{
		if (!parent->videoExporter || parent->videoExporter->complete)
			parent->confirmVideoExport();
		else
			parent->cancelVideoExport();
	});
	selectFileButton.signal_clicked().connect([this]
	{
		getFileLocationFromUser();
	});
	widthEntry.connectInputHandler([this]
	{
		uint32_t w;
		ConvUtils::updatePosIntIndicator(widthEntry, w, parent->DefaultVideoWidth, parent->MaxVideoWidth);
		if (widthEntry.hasValidInput() && w % 4 != 0)
			widthEntry.indicateInvalid("must be divisible by 4");
		else
			parent->videoExportWidth = w;
		parent->vexpEntryValidityChangeListeners.invoke();
	});
	heightEntry.connectInputHandler([this]
	{
		uint32_t h;
		ConvUtils::updatePosIntIndicator(heightEntry, h, parent->DefaultVideoHeight, parent->MaxVideoHeight);
		if (heightEntry.hasValidInput() && h % 4 != 0)
			heightEntry.indicateInvalid("must be divisible by 4");
		else
			parent->videoExportHeight = h;
		parent->vexpEntryValidityChangeListeners.invoke();
	});
	bitrateEntry.connectInputHandler([this]
	{
		ConvUtils::updatePosIntIndicator(bitrateEntry, parent->videoExportBitrate, parent->DefaultVideoBitrate, parent->MaxVideoBitrate);
		parent->vexpEntryValidityChangeListeners.invoke();
	});
	signal_delete_event().connect([this](GdkEventAny*)
	{
		parent->leaveVideoExport();
		return false;
	});
}

void ExportWindow::connectStateEventHandlers()
{
	parent->vexpEnterListeners.plug([this]
	{
		init();
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
	parent->vexpPlaybackSpeedupChangeListeners.plug([this]
	{
		updatePlaybackSpeedLabel();
		updateDurationLabel();
	});
	parent->updateListeners.plug([this]
	{
		if (parent->inVideoExport)
			update();
	});
	parent->vexpRangeValidityChangeListeners.plug([this]
	{
		updateInvalidTimesWarn();
		updateExportButtonSensitivity();
	});
	parent->vexpPlaybackStateChangeListeners.plug([this]
	{
		updatePlayPauseButtonLabel();
	});
	parent->vexpExportUpdateListeners.plug([this]
	{
		updateProgressIndicators();
	});
	parent->vexpFileLocationChangeListeners.plug([this]
	{
		updateFileLocationLabel();
	});
	parent->vexpEntryFieldValidators.plug([this]
	{
		return widthEntry.hasValidInput() && heightEntry.hasValidInput() && bitrateEntry.hasValidInput();
	});
	parent->vexpEntryValidityChangeListeners.plug([this]
	{
		updateExportButtonSensitivity();
	});
	parent->vexpExportStateChangeListeners.plug([this]
	{
		updateExportButtonLabel();
		updateProgressIndicators();
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
		const double realtime = simtime / (parent->params->dt * parent->params->stepsPerFrame * parent->videoExportPlaybackSpeedup) * parent->MsPerBaseFrame / 1000;
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
	exportButton.set_sensitive(parent->videoExportRangeValid && parent->vexpEntryFieldValidators.isAllValid());
}

void ExportWindow::setTimeScaleFromTime()
{
	if (parent->videoExportRangeValid)
	{
		const double fromStart = parent->videoExportTime - parent->videoExportStartTime;
		const double range = parent->videoExportEndTime - parent->videoExportStartTime;
		timeScale.quietSetValue(fromStart / range);
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

void ExportWindow::updateExportButtonLabel()
{
	if (parent->videoExporter && !parent->videoExporter->complete)
		exportButton.set_label("cancel export");
	else
		exportButton.set_label(exportButtonDefaultLabel);
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
	updateExportButtonLabel();
}

}