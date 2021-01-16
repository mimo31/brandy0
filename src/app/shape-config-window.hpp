/**
 * shape-config-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#ifndef SHAPE_CONFIG_WINDOW_HPP
#define SHAPE_CONFIG_WINDOW_HPP

#include <gtkmm/button.h>
#include <gtkmm/grid.h>

#include "annotated-entry.hpp"
#include "brandy-window.hpp"
#include "shape-config-widget.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class ShapeConfigWindow : public BrandyWindow
{
private:
    Gtk::Grid rootGrid;
    AnnotatedEntry widthEntry, heightEntry;
    Gtk::Button undoButton, redoButton, polygonDoneButton;
    ShapeConfigWidget shapeWidget;
	SimulatorParams *params;

	void updateUndoRedoSensitivity();

public:
    ShapeConfigWindow(const std::function<void(bool)>& scalarsValidityChangedCallback);

    void setParamsLocation(SimulatorParams *const);
    void setFromParams();
	void writeObstaclesToParams();
	void refreshGridSize();
};

}

#endif // SHAPE_CONFIG_WINDOW_HPP