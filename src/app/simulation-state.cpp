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
	win = new SimulationWindow([=](){
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
	SimulatorParams p = params;
	p.bcx0.u.y = .01;
	setParams(p);
	win->setParams(this->params);
	addLastFrame();
	win->setCurFrame(sim->f1);
	startComputeThread();
	Glib::signal_timeout().connect(sigc::mem_fun(*this, &SimulationState::runRedrawThread), 40);
	showWindow();
}

void SimulationState::deactivate()
{
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
	if (this->sim == nullptr)
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
			break;
		checkCapacity();
		framesMutex.unlock();
		if (stopComputingSignal)
		{
			stopComputingSignal = false;
			break;
		}
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

bool SimulationState::runRedrawThread()
{
	//while (true)
	//{
		framesMutex.lock();
		const uint32_t frame = uint32_t(drawFrame / frameStepSize + .5);
		win->setCurFrame(frames[frame]);
		cout << "set frame " << frame << endl;
		drawFrame++;
		if (drawFrame >= frameCount)
			drawFrame = 0;
		framesMutex.unlock();
		win->redraw();
		cout << "redrawn" << endl;
		return true;
		//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	//}
}

void SimulationState::startRedrawThread()
{
	redrawThread = std::thread([=](){
		runRedrawThread();
	});
}

}
