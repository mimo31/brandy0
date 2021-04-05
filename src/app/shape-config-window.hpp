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

/**
 * Class representing the shape configuration window.
 */
class ShapeConfigWindow : public ShapeConfigWindowAbstr, public BrandyWindow
{
private:
	/// Window's main grid
    Gtk::Grid rootGrid;

	/// Frame for the section with physical dimensions entries
	Gtk::Frame dimensionsFrame;
	/// Grid for the sectino with physical dimensions entries
	Gtk::Grid dimensionsGrid;
	/// Entry for the physical width
    AnnotatedEntry widthEntry;
	/// Entry for the physical height
	AnnotatedEntry heightEntry;

	/// Label explaining that the shape adding mode selector sets that mode
	Gtk::Label addingShapeLabel;
	/// Combo box for selecting the shape adding mode
	Gtk::ComboBoxText shapeSelector;
	/// Frame for the section with controls and information regarding the shape that is being added
	Gtk::Frame addingShapeFrame;
	/// Grid for the section with controls and informatino regarding the shape that is being added
	Gtk::Grid addingShapeGrid;
	/// Label displaying how many vertices of a polygon have already been set
	Hideable<Gtk::Label> polygonVerticesLabel;
	/// Button that clears everything that has been set for the current shape
	Gtk::Button clearShapeButton;
	/// Button that removes the last vertex added to a polygon
	Hideable<Gtk::Button> polygonPopVertexButton;
	/// Button that ends the creation of a polygon and adds the polygon to the shape stack
	Hideable<Gtk::Button> polygonFinishButton;

	/// Frame for the section with general shape stack control
	Gtk::Frame generalFrame;
	/// Grid for the section with general shape stack control
	Gtk::Grid generalGrid;
	/// Button for undoing the addition of the last shape
    Gtk::Button undoButton;
	/// Button for redoing the previously undone additions of shapes
	Gtk::Button redoButton;
	/// Button for irreversibly removing all shapes from the shape stack
	Gtk::Button clearAllButton;

	/// Main interactive widget for shape configuration
    ShapeConfigWidget shapeWidget;
	/// Pointer to the parent (abstract) configuration state
	ConfigStateAbstr *parent;

	/**
	 * Sets the sensitivity of the buttons for the general control of the shape stack (undo, redo, clear)
	 */
	void updateGeneralSensitivity();
	/**
	 * Sets the contents of the entries for physical dimensions (width and height) based on the data stored in *(parent->params)
	 */
    void setEntryFields();

	/**
	 * Updates the visibility, sensitivity, and content of the widgets related to the shape that is currently being added
	 */
	void updateAddShapeWidgets();

	/**
	 * Connects appropriate handlers to signals and events produced by this window and its widgets
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectWindowEventHandlers();
	/**
	 * Connects appropriate handlers to events managed by the parent state
	 * (meant to be called exactly once, in the constructor)
	 */
	void connectStateEventHandlers();
public:
	/**
	 * Constructs a ShapeConfigWindow object
	 * @param parent pointer to the parent (abstract) configuration state
	 */
    ShapeConfigWindow(ConfigStateAbstr *parent);
};

}

#endif // SHAPE_CONFIG_WINDOW_HPP