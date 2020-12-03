/**
 * start-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef START_WINDOW_HPP
#define START_WINDOW_HPP

#include <functional>

#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "glob.hpp"

namespace brandy0
{

class StartWindow : public Gtk::Window
{
private:
	Gtk::Button newButton, loadButton;
	Gtk::Grid layoutGrid;

public:
	StartWindow(const std::function<void()>& newSimCallback,
			const std::function<void(std::string)>& loadSimCallback);
	~StartWindow() override;
	//void show();
	//void hide();
};

}

#endif // START_WINDOW_HPP
