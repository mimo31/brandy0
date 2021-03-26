/**
 * about-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#include "about-window.hpp"

#include "style-manager.hpp"

namespace brandy0
{

AboutWindow::AboutWindow(StartStateAbstr *const parent)
	: parent(parent),
	authorLabel("Developed by Viktor Fukala from October 2020 to March 2021."),
	purposeLabel("Created as a final project for the \"Maturita\" exam."),
	repoLabel("GitHub repository at https://github.com/mimo31/brandy0 ."),
	librariesFrame("libraries"),
	librariesLabel("This software makes use of the following external libraries."),
	gtkmmNameLabel("gtkmm 3"),
	gtkmmLinkLabel("https://gitlab.gnome.org/GNOME/gtkmm"),
	ffmpegNameLabel("libavcodec, libavformat, libavutil, libswscale (FFmpeg libraries)"),
	ffmpegLinkLabel("https://www.ffmpeg.org/")
{
	set_resizable(false);

	mainGrid.set_orientation(Gtk::ORIENTATION_VERTICAL);

	mainGrid.add(authorLabel);
	mainGrid.add(purposeLabel);
	mainGrid.add(repoLabel);
	mainGrid.set_row_spacing(StyleManager::WidgetSpacing);

	librariesGrid.attach(librariesLabel, 0, 0, 2, 1);
	gtkmmNameLabel.set_xalign(0);
	gtkmmLinkLabel.set_xalign(0);
	ffmpegNameLabel.set_xalign(0);
	ffmpegLinkLabel.set_xalign(0);
	librariesGrid.attach(gtkmmNameLabel, 0, 1);
	librariesGrid.attach(gtkmmLinkLabel, 1, 1);
	librariesGrid.attach(ffmpegNameLabel, 0, 2);
	librariesGrid.attach(ffmpegLinkLabel, 1, 2);
	librariesGrid.set_row_spacing(5);
	librariesGrid.set_column_spacing(5);
	StyleManager::setPadding(librariesGrid);
	librariesFrame.add(librariesGrid);
	mainGrid.add(librariesFrame);

	add(mainGrid);

	show_all_children();
}

}