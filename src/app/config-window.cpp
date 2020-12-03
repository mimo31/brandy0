/**
 * config-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-window.hpp"

namespace brandy0
{

ConfigWindow::ConfigWindow(const std::function<void()>& backHomeCallback)
	: backHomeButton("back to home"),
	gridWidthLabel("grid width:")
{
	set_title("brandy0");
	set_border_width(12);

	backHomeButton.signal_clicked().connect(backHomeCallback);

	layoutGrid.attach(gridWidthLabel, 0, 0, 1, 1);
	layoutGrid.attach(gridWidthEntry, 1, 0, 1, 1);
	layoutGrid.attach(backHomeButton, 0, 1, 1, 1);

	add(layoutGrid);

	show_all_children();
}

ConfigWindow::~ConfigWindow()
{
}

}
