/**
 * start-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#include "start-window.hpp"

#include <iostream>

namespace brandy0
{

StartWindow::StartWindow(const std::function<void()>& newSimCallback,
			const std::function<void(std::string)>& loadSimCallback)
	: newButton("new simulation"),
	loadButton("load simulation"),
	descriptionLabel("Welcome to brandy0, a fluid dynamics simulator!")
{
	set_title("brandy0");
	set_border_width(12);

	newButton.signal_clicked().connect(newSimCallback);

	layoutGrid.set_orientation(Gtk::ORIENTATION_VERTICAL);

	layoutGrid.add(descriptionLabel);
	layoutGrid.add(newButton);
	layoutGrid.add(loadButton);

	layoutGrid.set_row_spacing(5);

	add(layoutGrid);

	show_all_children();
}

StartWindow::~StartWindow()
{
}

}
