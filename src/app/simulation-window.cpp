/**
 * simulation-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#include "simulation-window.hpp"

namespace brandy0
{

SimulationWindow::SimulationWindow(const std::function<void()>& backToConfigCallback)
	: toConfigButton("back to config")
{
	set_title("brandy0");
	set_default_size(1280, 720);
	set_border_width(12);

	mainGrid.attach(dArea, 0, 0, 1, 1);

	toConfigButton.signal_clicked().connect(backToConfigCallback);

	panelGrid.attach(toConfigButton, 0, 0, 1, 3);
	panelGrid.attach(timeLabel, 1, 0, 1, 1);
	panelGrid.attach(simulatedToTimeLabel, 1, 1, 1, 1);
	panelGrid.attach(storedFramesLabel, 1, 2, 1, 1);

	mainGrid.attach(panelGrid, 0, 1, 1, 1);

	add(mainGrid);

	show_all_children();
}

void SimulationWindow::setParams(const SimulatorParams *const params)
{
	dArea.setParams(params);
}

void SimulationWindow::setCurFrame(const SimFrame& curFrame)
{
	dArea.setCurFrame(curFrame);
}

void SimulationWindow::redraw()
{
	dArea.redraw();
}

void SimulationWindow::setTime(const double time, const double of)
{
	if (of != 0)
		timeLabel.set_text("t = " + std::to_string(time) + " (" + std::to_string(time / of * 100) + " %)");
	else
		timeLabel.set_text("t = " + std::to_string(time));
}

void SimulationWindow::setSimulatedToTime(const double simulatedTo)
{
	simulatedToTimeLabel.set_text("computed up to t = " + std::to_string(simulatedTo));
}

void SimulationWindow::setStoredFrames(const uint32_t stored, const uint32_t capacity)
{
	storedFramesLabel.set_text("stored frames: " + std::to_string(stored) + " / " + std::to_string(capacity));
}

}
