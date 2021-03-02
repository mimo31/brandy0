/**
 * about-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#include "about-window.hpp"

namespace brandy0
{

AboutWindow::AboutWindow(StartStateAbstr *const parent)
	: parent(parent),
	authorLabel("Developed by Viktor Fukala from October 2020 to March 2021."),
	purposeLabel("Created as a final project for the \"Maturita\" exam."),
	repoLabel("GitHub repository at https://github.com/mimo31/brandy0 .")
{
	mainGrid.set_orientation(Gtk::ORIENTATION_VERTICAL);

	mainGrid.add(authorLabel);
	mainGrid.add(purposeLabel);
	mainGrid.add(repoLabel);

	add(mainGrid);

	show_all_children();
}

}