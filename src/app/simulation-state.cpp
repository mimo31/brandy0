/**
 * simulation-state.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#include "simulation-state.hpp"

#include <chrono>

#include <glibmm.h>

#include "simulator-classic.hpp"

namespace brandy0
{

SimulationState::SimulationState(ApplicationAbstr *const app)
	: SimulationStateAbstr(app),
	mainWin(this),
	exportWin(this)
{
}

void SimulationState::start(const SimulationParams& params)
{
	frameCount = 0;
	frameStepSize = 1;
	time = 0;
	computedIter = 0;
	editingTime = false;
	playbackPaused = false;
	playbackSpeedup = 1;
	inVideoExport = false;
	computing = false;
	crashed = false;
	crashSignal = false;
	frames.clear();
	this->params = make_unique<SimulationParams>(params);
	sim = make_unique<SimulatorClassic>(params);
	sim->setPauseControl(&stopComputingSignal, &computingMutex);
	frontDisplayMode = FrontDisplayModeDefault;
	backDisplayMode = BackDisplayModeDefault;
	playbackMode = defaultPlaybackMode;
	addLastFrame();
	initListeners.invoke();
	resumeComputation();
	lastUpdate = std::chrono::steady_clock::now();
	updateConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SimulationState::update), 40);
}

void SimulationState::activate(const SimulationParams& params)
{
	start(params);
	showMainWindow();
}

void SimulationState::run(const SimulationParams &params, const uint32_t frames)
{
	closeAfterFrames = frames;
	start(params);
	mainWin.show();
	app->run(mainWin);
}

void SimulationState::deactivate()
{
	leaveVideoExport();
	exportWin.hide();
	computingMutex.lock();
	if (computing)
		stopComputingSignal = true;
	computingMutex.unlock();
	if (computeThread.joinable())
		computeThread.join();
	updateConnection.disconnect();
	mainWin.hide();
}

void SimulationState::goBackToConfig()
{
	app->enterExistingConfig(*params);
}

void SimulationState::pauseComputation()
{
	computingMutex.lock();
	const bool paused = computing;
	if (computing)
		stopComputingSignal = true;
	computingMutex.unlock();
	if (computeThread.joinable())
		computeThread.join();
	if (paused)
		computingSwitchListeners.invoke();
}

void SimulationState::resumeComputation()
{
	computingMutex.lock();
	const bool started = !computing;
	if (!computing)
		startComputeThread();
	computingMutex.unlock();
	if (started)
		computingSwitchListeners.invoke();
}

void SimulationState::closeAll()
{
	if (inVideoExport)
	{
		leaveVideoExport();
		exportWin.close();
	}
}

void SimulationState::enterVideoExport()
{
	inVideoExport = true;
	pauseComputation();
	playbackPaused = true;
	playbackStateChangeListeners.invoke();
	updateComputedTime();
	videoExportStartTime = 0;
	videoExportEndTime = computedTime;
	videoExportTime = 0;
	videoExportPlaybackSpeedup = 1;
	videoExportRangeValid = computedTime != 0;
	videoExportPlaybackPaused = true;
	videoExportEditingTime = false;
	videoExportFileLocation = "./exported.mp4";
	videoExportWidth = DefaultVideoWidth;
	videoExportHeight = DefaultVideoHeight;
	videoExportBitrate = DefaultVideoBitrate;
	vexpEnterListeners.invoke();
	showExportWindow();
}

void SimulationState::leaveVideoExport()
{
	if (inVideoExport)
	{
		inVideoExport = false;
		cancelVideoExport();
		vexpLeaveListeners.invoke();
	}
}

void SimulationState::confirmVideoExport()
{
	videoExporter = make_unique<VideoExporter>(
		*params,
		backDisplayMode,
		frontDisplayMode,
		videoExportFileLocation,
		frames,
		videoExportStartTime,
		videoExportEndTime,
		MsPerBaseFrame / videoExportPlaybackSpeedup * frameStepSize,
		params->dt * params->stepsPerFrame * frameStepSize,
		videoExportWidth,
		videoExportHeight,
		videoExportBitrate,
		&app->graphicsManager);
	videoExporter->updateListeners.plug([this]
	{
		vexpExportUpdateListeners.invoke();
		if (videoExporter->complete)
			vexpExportStateChangeListeners.invoke();
	});
	vexpExportStateChangeListeners.invoke();
	videoExporter->exportVideo();
}

void SimulationState::cancelVideoExport()
{
	if (videoExporter)
	{
		if (!videoExporter->complete)
			videoExporter->cancel();
		videoExporter = nullptr;
		vexpExportStateChangeListeners.invoke();
	}
}

bool SimulationState::isComputing()
{
	computingMutex.lock();
	const bool ret = computing;
	computingMutex.unlock();
	return ret;
}

uint32_t SimulationState::getFramesStored()
{
	framesMutex.lock();
	const uint32_t ret = frames.size();
	framesMutex.unlock();
	return ret;
}

uint32_t SimulationState::getComputedIter()
{
	framesMutex.lock();
	const uint32_t ret = computedIter;
	framesMutex.unlock();
	return ret;
}

void SimulationState::showMainWindow()
{
	app->addWindow(mainWin);
	mainWin.show();
}

void SimulationState::showExportWindow()
{
	app->addWindow(exportWin);
	exportWin.show();
}

void SimulationState::checkCapacity()
{
	if (frames.size() == params->frameCapacity)
	{
		vec<SimFrame> oldframes = frames;
		frames.clear();
		for (uint32_t i = 0; i < oldframes.size(); i++)
		{
			if (i % 2 == 0)
				frames.push_back(oldframes[i]);
		}
		frameStepSize *= 2;
	}
}

double SimulationState::getTime(const uint32_t frame)
{
	return params->dt * params->stepsPerFrame * frame;
}

void SimulationState::addLastFrame()
{
	if (frameCount % frameStepSize == 0)
	{
		frames.push_back(sim->f1);
	}
	frameCount++;
}

void SimulationState::runComputeThread()
{
	const auto updatefreq = std::chrono::milliseconds(17);
	auto lastupdate = std::chrono::steady_clock::now();
	framesMutex.lock();
	uint32_t startiter = computedIter;
	framesMutex.unlock();
	while (true)
	{
		bool stop = false;
		for (uint32_t i = startiter; i < params->stepsPerFrame; i++)
		{
			auto ctime = std::chrono::steady_clock::now();
			if (ctime - lastupdate > updatefreq)
			{
				framesMutex.lock();
				computedIter = i;
				framesMutex.unlock();
				computingMutex.lock();
				if (stopComputingSignal)
				{
					stopComputingSignal = false;
					computingMutex.unlock();
					stop = true;
					break;
				}
				computingMutex.unlock();
				lastupdate = ctime;
			}
			sim->iter();
			if (sim->crashed)
			{
				crashMutex.lock();
				crashSignal = true;
				crashMutex.unlock();
				framesMutex.lock();
				computedIter = i;
				framesMutex.unlock();
				stop = true;
				break;
			}
			if (sim->incomplete)
			{
				computingMutex.lock();
				stopComputingSignal = false;
				computingMutex.unlock();
				framesMutex.lock();
				computedIter = i;
				framesMutex.unlock();
				stop = true;
				break;
			}
		}
		if (stop)
			break;
		startiter = 0;
		framesMutex.lock();
		addLastFrame();
		computedIter = 0;
		if (params->stopAfter >= 0 && getTime(frameCount) >= params->stopAfter && getTime(frameCount - 1) < params->stopAfter)
		{
			framesMutex.unlock();
			break;
		}
		checkCapacity();
		framesMutex.unlock();
		computingMutex.lock();
		if (stopComputingSignal)
		{
			stopComputingSignal = false;
			computingMutex.unlock();
			break;
		}
		computingMutex.unlock();
	}
	computingMutex.lock();
	computing = false;
	computingMutex.unlock();
}

void SimulationState::startComputeThread()
{
	computing = true;
	computeThread = std::thread([=]{
		runComputeThread();
	});
}

void SimulationState::updateComputedTime()
{
	computedTime = (frames.size() - 1) * frameStepSize * params->stepsPerFrame * params->dt;
}

bool SimulationState::update()
{
	std::chrono::steady_clock::time_point ctime = std::chrono::steady_clock::now();
	const double elapsedms = std::chrono::duration<double, std::chrono::milliseconds::period>(ctime - lastUpdate).count();
	lastUpdate = ctime;

	crashMutex.lock();
	if (crashSignal)
	{
		crashed = true;
		crashSignal = false;
		crashListeners.invoke();
	}
	crashMutex.unlock();

	if (!inVideoExport)
	{
		framesMutex.lock();
		updateComputedTime();
		if (!editingTime && !playbackPaused)
		{
			if (playbackMode == PlaybackMode::PlayUntilEnd || playbackMode == PlaybackMode::Loop)
			{
				time += params->stepsPerFrame * params->dt * playbackSpeedup * elapsedms / MsPerBaseFrame;
				if (time > computedTime)
				{
					if (playbackMode == PlaybackMode::PlayUntilEnd)
						time = computedTime;
					else
						if (computedTime == 0)
							time = 0;
						else
							time -= computedTime * floor(time / computedTime);
				}
			}
			else
				time = computedTime;
		}
		curFrame = make_unique<SimFrame>(frames[getFrameNumber(time)]);
		if (closeAfterFrames && (frames.size() - 1) * frameStepSize >= closeAfterFrames)
		{
			framesMutex.unlock();
			pauseComputation();
			mainWin.close();
		}
		else
			framesMutex.unlock();
	}
	else
	{
		if (!videoExportPlaybackPaused && !videoExportEditingTime)
		{
			videoExportTime += params->stepsPerFrame * params->dt * videoExportPlaybackSpeedup * elapsedms / MsPerBaseFrame;
			if (videoExportTime > videoExportEndTime)
				videoExportTime = videoExportEndTime;
		}
		curFrame = make_unique<SimFrame>(frames[getFrameNumber(videoExportTime)]);
	}

	updateListeners.invoke();
	return true;
}

uint32_t SimulationState::getFrameNumber(const double t)
{
	uint32_t frame = round(t / (params->stepsPerFrame * params->dt) / frameStepSize);
	if (frame >= frames.size())
		frame = frames.size() - 1;
	return frame;
}

void SimulationState::videoExportValidateRange()
{
	const bool valid = videoExportStartTime < videoExportEndTime;
	if (valid != videoExportRangeValid)
	{
		videoExportRangeValid = valid;
		vexpRangeValidityChangeListeners.invoke();
	}
}

void SimulationState::videoExportClampTime()
{
	videoExportTime = std::max(videoExportTime, videoExportStartTime);
	videoExportTime = std::min(videoExportTime, videoExportEndTime);
}

}
