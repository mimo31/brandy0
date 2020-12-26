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
	mainGrid.attach(toConfigButton, 0, 1, 1, 1);

	add(mainGrid);

	show_all_children();
}

void SimulationWindow::setParams(const SimulatorParams *const params)
{
	dArea.setParams(params);
}

void SimulationWindow::setCurFrame(const SimFrame *const curFrame)
{
	dArea.setCurFrame(curFrame);
}


}
