/**
 * about-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#ifndef ABOUT_WINDOW_HPP
#define ABOUT_WINDOW_HPP

#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "brandy-window.hpp"
#include "start-state-abstr.hpp"

namespace brandy0
{

/// The window containing the about information of this project
class AboutWindow : public BrandyWindow
{
private:
	/// Parent start state
	StartStateAbstr *parent;

	/// Grid containing all widgets; the direct child of the window
	Gtk::Grid mainGrid;
	/// Label with information about the author and the time of development
	Gtk::Label authorLabel;
	/// Label with information about the main purpose of this project
	Gtk::Label purposeLabel;
	/// Label with a link to the project repository
	Gtk::Label repoLabel;

	Gtk::Frame librariesFrame;
	Gtk::Grid librariesGrid;
	Gtk::Label librariesLabel;
	Gtk::Label gtkmmNameLabel;
	Gtk::Label gtkmmLinkLabel;
	Gtk::Label ffmpegNameLabel;
	Gtk::Label ffmpegLinkLabel;
	
public:
	/**
	 * Constructs the about window.
	 * @param parent the parent start state
	 */
	AboutWindow(StartStateAbstr *parent);
};

}

#endif // ABOUT_WINDOW_HPP