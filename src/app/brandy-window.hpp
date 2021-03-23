/**
 * brandy-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef BRANDY_WINDOW_HPP
#define BRANDY_WINDOW_HPP

#include <gtkmm/window.h>

namespace brandy0
{

class BrandyWindow : public Gtk::Window
{
protected:
    BrandyWindow();
    BrandyWindow(const uint32_t defaultWidth, const uint32_t defaultHeight);
};

}

#endif // BRANDY_WINDOW_HPP