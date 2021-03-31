/**
 * application.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "application.hpp"

#include "simulation-params-preset.hpp"

namespace brandy0
{

Application::Application()
	: 
	gtkapp(Gtk::Application::create("")),
	startSt(this),
	configSt(this),
	simulationSt(this)
{
}

Application::~Application()
{
	activeSt->deactivate();
	graphicsManager.destruct();
}

void Application::run(const int argc, const char *const *const argv)
{
	// look for an argument "--time" and parse arguments after it to run a simulation directly
	// (used mainly for automated time measurements of simulation run time)
	for (int i = 1; i < argc; i++)
	{
		const str arg = argv[i];
		if (arg == "--time")
		{
			if (i >= argc - 2)
				return;
			const str presetname = argv[i + 1];
			const str framesarg = argv[i + 2];
			uint32_t frames = 0;
			constexpr uint32_t limit = 100'000'000;
			for (const char c : framesarg)
			{
				if (c > '9' || c < '0')
					return;
				frames = frames * 10 + c - '0';
				if (frames > limit)
					return;
			}
			activeSt = &simulationSt;
			uptr<SimulationParams> preset;
			for (const SimulationParamsPreset &p : SimulationParamsPreset::Presets)
			{
				if (p.name == presetname)
					preset = make_unique<SimulationParams>(p.params);
			}
			if (preset)
				simulationSt.run(*preset, frames);
			return;
		}
	}

	// starts the application normally iff there was no "--time" argument
	activeSt = &startSt;
	startSt.run();
}

void Application::switchStates(State *const newSt)
{
	activeSt->deactivate();
	activeSt = newSt;
}

void Application::enterNewConfig()
{
	switchStates(&configSt);
	configSt.activate();
}

void Application::enterExistingConfig(const SimulationParams params)
{
	switchStates(&configSt);
	configSt.activate(params);
}

void Application::enterHome()
{
	switchStates(&startSt);
	startSt.activate();
}

void Application::enterNewSimulation(const SimulationParams params)
{
	switchStates(&simulationSt);
	simulationSt.activate(params);
}

void Application::addWindow(Gtk::Window& win)
{
	gtkapp->add_window(win);
}

void Application::run(Gtk::Window& win)
{
	gtkapp->run(win);
}

}
