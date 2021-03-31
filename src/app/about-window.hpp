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

	/// Frame containing the list of the external libraries used
	Gtk::Frame librariesFrame;
	/// Grid containing the list of the external libraries used
	Gtk::Grid librariesGrid;
	/// Label explaining that what follows is the list of external libraries used
	Gtk::Label librariesLabel;
	/// Label with the name of the gtkmm library
	Gtk::Label gtkmmNameLabel;
	/// Label with the link to the gtkmm repository
	Gtk::Label gtkmmLinkLabel;
	/// Label with the name of the used FFmpeg libraries
	Gtk::Label ffmpegNameLabel;
	/// Label with the link to the FFmpeg website
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