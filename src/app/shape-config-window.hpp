/**
 * shape-config-window.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#ifndef SHAPE_CONFIG_WINDOW_HPP
#define SHAPE_CONFIG_WINDOW_HPP

#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/stack.h>

#include "annotated-entry.hpp"
#include "brandy-window.hpp"
#include "config-state-abstr.hpp"
#include "hideable.hpp"
#include "shape-config-widget.hpp"
#include "shape-config-window-abstr.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

class ShapeConfigWindow : public ShapeConfigWindowAbstr, public BrandyWindow
{
private:
    Gtk::Grid rootGrid;

	Gtk::Frame dimensionsFrame;
	Gtk::Grid dimensionsGrid;
    AnnotatedEntry widthEntry, heightEntry;

	Gtk::Label addingShapeLabel;
	Gtk::ComboBoxText shapeSelector;
	Gtk::Frame addingShapeFrame;
	Gtk::Grid addingShapeGrid;
	Hideable<Gtk::Label> polygonVerticesLabel;
	Gtk::Button clearShapeButton;
	Hideable<Gtk::Button> polygonPopVertexButton, polygonFinishButton;

	Gtk::Frame generalFrame;
	Gtk::Grid generalGrid;
    Gtk::Button undoButton, redoButton, clearAllButton;

    ShapeConfigWidget shapeWidget;
	ConfigStateAbstr *parent;

	void updateGeneralSensitivity();
    void setEntryFields();

	void updateAddShapeWidgets();

public:
    ShapeConfigWindow(ConfigStateAbstr *parent);
};

}

#endif // SHAPE_CONFIG_WINDOW_HPP