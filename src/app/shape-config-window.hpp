/**
 * shape-config-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#ifndef SHAPE_CONFIG_WINDOW_HPP
#define SHAPE_CONFIG_WINDOW_HPP

#include <gtkmm/grid.h>

#include "annotated-entry.hpp"
#include "brandy-window.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class ShapeConfigWindow : public BrandyWindow
{
private:
    Gtk::Grid rootGrid;
    AnnotatedEntry widthEntry, heightEntry;

public:
    void setParamsLocation(SimulatorParams *const);
    void setFromParams();
};

}

#endif // SHAPE_CONFIG_WINDOW_HPP