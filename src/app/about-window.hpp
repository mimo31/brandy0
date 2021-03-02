/**
 * about-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#ifndef ABOUT_WINDOW_HPP
#define ABOUT_WINDOW_HPP

#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "brandy-window.hpp"
#include "start-state-abstr.hpp"

namespace brandy0
{

class AboutWindow : public BrandyWindow
{
private:
	StartStateAbstr *parent;

	Gtk::Grid mainGrid;
	Gtk::Label authorLabel, purposeLabel, repoLabel;
public:
	AboutWindow(StartStateAbstr *const);
};

}

#endif // ABOUT_WINDOW_HPP