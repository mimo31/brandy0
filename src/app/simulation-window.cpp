/**
 * simulation-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#include "simulation-window.hpp"

#include "display-modes.hpp"

namespace brandy0
{

SimulationWindow::SimulationWindow(const std::function<void()>& backToConfigCallback)
	: BrandyWindow(1280, 720), toConfigButton("back to config"), viewFrame("view setup")
{
	mainGrid.attach(dArea, 0, 0, 1, 1);

	for (uint32_t i = 0; i < BACK_DISPLAY_MODE_COUNT; i++)
		backDisplaySelector.append(BACK_DISPLAY_MODES[i].name);
	backDisplaySelector.set_active(BACK_DISPLAY_MODE_DEFAULT);
	backDisplaySelector.signal_changed().connect([this]()
	{
		dArea.setBackDisplayMode(backDisplaySelector.get_active_row_number());
	});

	for (uint32_t i = 0; i < FRONT_DISPLAY_MODE_COUNT; i++)
		frontDisplaySelector.append(FRONT_DISPLAY_MODES[i].name);
	frontDisplaySelector.set_active(FRONT_DISPLAY_MODE_DEFAULT);
	frontDisplaySelector.signal_changed().connect([this]()
	{
		dArea.setFrontDisplayMode(frontDisplaySelector.get_active_row_number());
	});

	viewGrid.attach(backDisplaySelector, 0, 0);
	viewGrid.attach(frontDisplaySelector, 0, 1);
	viewFrame.add(viewGrid);

	toConfigButton.signal_clicked().connect(backToConfigCallback);

	panelGrid.attach(toConfigButton, 0, 0, 1, 3);
	panelGrid.attach(timeLabel, 1, 0, 1, 1);
	panelGrid.attach(simulatedToTimeLabel, 1, 1, 1, 1);
	panelGrid.attach(storedFramesLabel, 1, 2, 1, 1);
	panelGrid.attach(viewFrame, 2, 0, 1, 3);

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
