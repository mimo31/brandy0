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

StartWindow::StartWindow(StartStateAbstr *const parent)
	: parent(parent),
	newButton("new simulation"),
	aboutButton("about"),
	descriptionLabel("Welcome to brandy0, a fluid dynamics simulator!")
{
	newButton.signal_clicked().connect([parent]
	{
		parent->goToNewSimulation();
	});
	aboutButton.signal_clicked().connect([parent]
	{
		parent->showAbout();
	});

	layoutGrid.set_orientation(Gtk::ORIENTATION_VERTICAL);

	layoutGrid.add(descriptionLabel);
	layoutGrid.add(newButton);
	layoutGrid.add(aboutButton);

	layoutGrid.set_row_spacing(5);

	add(layoutGrid);

	show_all_children();
}

}
