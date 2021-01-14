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

SimulationState::SimulationState(Application *const app) : app(app), params(nullptr), sim(nullptr)
{
	win = new SimulationWindow([=]()
	{
		app->enterExistingConfig(*params);
	});
}

SimulationState::~SimulationState()
{
	if (params != nullptr)
		delete params;
	if (sim != nullptr)
		delete sim;
	delete win;
}

void SimulationState::activate(const SimulatorParams& params)
{
	frameCount = 0;
	frameStepSize = 1;
	time = 0;
	frames.clear();
	setParams(params);
	win->setParams(this->params);
	addLastFrame();
	win->setCurFrame(sim->f1);
	startComputeThread();
	redrawConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &SimulationState::update), 40);
	showWindow();
}

void SimulationState::deactivate()
{
	computingMutex.lock();
	if (computing)
		stopComputingSignal = true;
	computingMutex.unlock();
	computeThread.join();
	redrawConnection.disconnect();
	win->hide();
}

void SimulationState::showWindow()
{
	app->gtkapp->add_window(*win);
	win->show();
}

void SimulationState::setParams(const SimulatorParams& params)
{
	if (this->params == nullptr)
		this->params = new SimulatorParams(params);
	else
		*(this->params) = params;
	if (this->sim != nullptr)
		delete this->sim;
	this->sim = new SimulatorClassic(params);
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
	computingMutex.lock();
	computing = true;
	computingMutex.unlock();
	while (true)
	{
		for (uint32_t i = 0; i < params->stepsPerFrame; i++)
			sim->iter();
		framesMutex.lock();
		addLastFrame();
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
	computeThread = std::thread([=](){
		runComputeThread();
	});
}

bool SimulationState::update()
{
	framesMutex.lock();
	uint32_t frame = uint32_t(time / (params->stepsPerFrame * params->dt) / frameStepSize + .5);
	if (frame >= frames.size())
	{
		frame = 0;
		time = 0;
	}
	win->setCurFrame(frames[frame]);

	const double timeToSet = time;
	const uint32_t storedFramesToSet = frames.size();
	const uint32_t capacityToSet = params->frameCapacity;
	const double simulatedTimeToSet = (storedFramesToSet - 1) * frameStepSize * params->stepsPerFrame * params->dt;
	
	time += params->stepsPerFrame * params->dt;
	framesMutex.unlock();

	win->setTime(timeToSet, simulatedTimeToSet);
	win->setSimulatedToTime(simulatedTimeToSet);
	win->setStoredFrames(storedFramesToSet, capacityToSet);

	win->redraw();
	return true;
}

}
