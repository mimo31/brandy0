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
	: app(app),
	mainWin(std::make_unique<SimulationWindow>(this)),
	exportWin(std::make_unique<ExportWindow>(this))
{
}

void SimulationState::activate(const SimulatorParams& params)
{
	frameCount = 0;
	frameStepSize = 1;
	time = 0;
	computedIter = 0;
	editingTime = false;
	playbackPaused = false;
	playbackSpeedup = 1;
	inVideoExport = false;
	frames.clear();
	this->params = std::make_unique<SimulatorParams>(params);
	sim = std::make_unique<SimulatorClassic>(params);
	frontDisplayMode = FRONT_DISPLAY_MODE_DEFAULT;
	backDisplayMode = BACK_DISPLAY_MODE_DEFAULT;
	playbackMode = defaultPlaybackMode;
	addLastFrame();
	initListeners.invoke();
	resumeComputation();
	redrawConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SimulationState::update), 40);
	showMainWindow();
}

void SimulationState::deactivate()
{
	computingMutex.lock();
	if (computing)
		stopComputingSignal = true;
	computingMutex.unlock();
	if (computeThread.joinable())
		computeThread.join();
	redrawConnection.disconnect();
	mainWin->hide();
}

void SimulationState::goBackToConfig()
{
	app->enterExistingConfig(*params);
}

void SimulationState::pauseComputation()
{
	computingMutex.lock();
	if (computing)
		stopComputingSignal = true;
	computingMutex.unlock();
	if (computeThread.joinable())
		computeThread.join();
	computingSwitchListeners.invoke();
}

void SimulationState::resumeComputation()
{
	computingMutex.lock();
	const bool started = !computing;
	if (!computing)
	{
		startComputeThread();
	}
	computingMutex.unlock();
	if (started)
		computingSwitchListeners.invoke();
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
	videoExportPlaybackSpeedup = 1;
	videoExportEnterListeners.invoke();
	showExportWindow();
}

void SimulationState::leaveVideoExport()
{
	inVideoExport = false;
	exportWin->hide();
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
	app->addWindow(*mainWin);
	mainWin->show();
}

void SimulationState::showExportWindow()
{
	app->addWindow(*exportWin);
	exportWin->show();
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
	computeThread = std::thread([=](){
		runComputeThread();
	});
}

void SimulationState::updateComputedTime()
{
	computedTime = (frames.size() - 1) * frameStepSize * params->stepsPerFrame * params->dt;
}

bool SimulationState::update()
{
	framesMutex.lock();
	updateComputedTime();
	if (!editingTime && !playbackPaused)
	{
		if (playbackMode == PlaybackMode::PLAY_UNTIL_END || playbackMode == PlaybackMode::LOOP)
		{
			time += params->stepsPerFrame * params->dt * playbackSpeedup;
			if (time > computedTime)
			{
				if (playbackMode == PlaybackMode::PLAY_UNTIL_END)
					time = computedTime;
				else
					time = 0;
			}
		}
		else
			time = computedTime;
	}
	uint32_t frame = uint32_t(time / (params->stepsPerFrame * params->dt) / frameStepSize + .5);
	if (frame >= frames.size())
		frame = frames.size() - 1;
	curFrame = std::make_unique<SimFrame>(frames[frame]);

	framesMutex.unlock();

	mainWin->update();
	return true;
}

}
