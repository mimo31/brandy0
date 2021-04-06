/**
 * shape-config-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#include "shape-config-window.hpp"

#include "conv-utils.hpp"
#include "simulation-params-preset.hpp"

namespace brandy0
{

void ShapeConfigWindow::updateGeneralSensitivity()
{
	undoButton.set_sensitive(parent->params->shapeStack.canUndo());
	redoButton.set_sensitive(parent->params->shapeStack.canRedo());
	clearAllButton.set_sensitive(!parent->params->shapeStack.empty());
}

void ShapeConfigWindow::setEntryFields()
{
	uptr<SimulationParams>& params = parent->params;
	widthEntry.setText(ConvUtils::defaultToString(params->w));
	heightEntry.setText(ConvUtils::defaultToString(params->h));
}

void ShapeConfigWindow::updateAddShapeWidgets()
{
	if (addShapeMode == AddShapePolygon)
	{
		polygonVerticesLabel.pseudoShow();
		const uint32_t vertSet = nextShapeClicks.size();
		const str labelText = std::to_string(vertSet) + " " + (vertSet == 1 ? "vertex" : "vertices") + " set";
		polygonVerticesLabel.set_text(labelText);
		polygonPopVertexButton.pseudoShow();
		polygonPopVertexButton.set_sensitive(vertSet != 0);
		polygonFinishButton.pseudoShow();
		polygonFinishButton.set_sensitive(vertSet >= 3);
	}
	else
	{
		polygonVerticesLabel.pseudoHide();
		polygonPopVertexButton.pseudoHide();
		polygonFinishButton.pseudoHide();
	}

	addingShapeFrame.set_label("current " + AddShapeModes[addShapeMode].name);
	clearShapeButton.set_sensitive(!nextShapeClicks.empty());
}

ShapeConfigWindow::ShapeConfigWindow(ConfigStateAbstr *parent)
	: BrandyWindow(1280, 720),
	dimensionsFrame("physical dimensions"),
	widthEntry("width:", &parent->app->styleManager),
	heightEntry("height:", &parent->app->styleManager),
	addingShapeLabel("adding shape"),
	addingShapeFrame("current rectangle"),
	polygonVerticesLabel("0 vertices set"),
	clearShapeButton("clear"),
	polygonPopVertexButton("pop vertex"),
	polygonFinishButton("finish"),
	generalFrame("general controls"),
	undoButton("undo"),
	redoButton("redo"),
	clearAllButton("clear all (can't undo!)"),
	shapeWidget(parent, this),
	parent(parent)
{
	widthEntry.attachTo(dimensionsGrid, 0, 0);
	heightEntry.attachTo(dimensionsGrid, 0, 1);

	dimensionsGrid.set_column_spacing(StyleManager::WidgetSpacing);
	StyleManager::setPadding(dimensionsGrid);

	dimensionsFrame.add(dimensionsGrid);
	rootGrid.attach(dimensionsFrame, 0, 0, 1, 2);

	rootGrid.attach(addingShapeLabel, 1, 0);
	rootGrid.attach(shapeSelector, 2, 0);

	addingShapeGrid.attach(polygonVerticesLabel, 0, 0, 2, 1);
	addingShapeGrid.attach(clearShapeButton, 0, 1);
	addingShapeGrid.attach(polygonPopVertexButton, 1, 1);
	addingShapeGrid.attach(polygonFinishButton, 0, 2, 2, 1);
	addingShapeGrid.set_column_spacing(StyleManager::WidgetSpacing);
	addingShapeGrid.set_row_spacing(StyleManager::WidgetSpacing);
	StyleManager::setPadding(addingShapeGrid);
	addingShapeFrame.add(addingShapeGrid);
	rootGrid.attach(addingShapeFrame, 1, 1, 2, 1);

	generalGrid.attach(undoButton, 0, 0);
	generalGrid.attach(redoButton, 0, 1);
	generalGrid.attach(clearAllButton, 0, 2);
	generalGrid.set_column_spacing(StyleManager::WidgetSpacing);
	generalGrid.set_row_spacing(StyleManager::WidgetSpacing);
	StyleManager::setPadding(generalGrid);
	generalGrid.set_valign(Gtk::ALIGN_CENTER);
	generalGrid.set_halign(Gtk::ALIGN_CENTER);
	generalFrame.add(generalGrid);
	rootGrid.attach(generalFrame, 3, 0, 1, 2);

	rootGrid.attach(shapeWidget, 0, 2, 4, 1);

	rootGrid.set_column_spacing(StyleManager::WidgetSpacing);
	rootGrid.set_row_spacing(StyleManager::WidgetSpacing);

	shapeWidget.show();

	for (const AddShapeMode mode : AddShapeModes)
		shapeSelector.append(mode.name);
	shapeSelector.set_active(AddShapeModeDefault);
	addShapeMode = AddShapeModeDefault;
	
	add(rootGrid);

	show_all_children();

	connectWindowEventHandlers();
	connectStateEventHandlers();
}

void ShapeConfigWindow::connectWindowEventHandlers()
{
	widthEntry.connectInputHandler([this]
	{
		ConvUtils::updatePosRealIndicator(widthEntry, parent->params->w, SimulationParamsPreset::DefaultW, SimulationParamsPreset::MinW, SimulationParamsPreset::MaxW);
		parent->validityChangeListeners.invoke();
		parent->dimensionsChangeListeners.invoke();
	});
	heightEntry.connectInputHandler([this]
	{
		ConvUtils::updatePosRealIndicator(heightEntry, parent->params->h, SimulationParamsPreset::DefaultH, SimulationParamsPreset::MinH, SimulationParamsPreset::MaxH);
		parent->validityChangeListeners.invoke();
		parent->dimensionsChangeListeners.invoke();
	});
	undoButton.signal_clicked().connect([this]
	{
		parent->params->shapeStack.undo();
		parent->shapeStackChangeListeners.invoke();
	});
	redoButton.signal_clicked().connect([this]
	{
		parent->params->shapeStack.redo();
		parent->shapeStackChangeListeners.invoke();
	});
	clearAllButton.signal_clicked().connect([this]
	{
		parent->params->shapeStack.clear();
		parent->shapeStackChangeListeners.invoke();
	});
	shapeSelector.signal_changed().connect([this]
	{
		const uint32_t newMode = shapeSelector.get_active_row_number();
		if (newMode != addShapeMode)
		{
			nextShapeClicks.clear();
			addShapeMode = newMode;
			nextShapeChangeListeners.invoke();
		}
	});
	clearShapeButton.signal_clicked().connect([this]
	{
		nextShapeClicks.clear();
		nextShapeChangeListeners.invoke();
	});
	polygonPopVertexButton.signal_clicked().connect([this]
	{
		if (addShapeMode == AddShapePolygon && !nextShapeClicks.empty())
		{
			nextShapeClicks.pop_back();
			nextShapeChangeListeners.invoke();
		}
	});
	polygonFinishButton.signal_clicked().connect([this]
	{
		if (addShapeMode == AddShapePolygon && nextShapeClicks.size() >= 3)
		{
			parent->params->shapeStack.push(make_shared<ObstaclePolygon>(false, nextShapeClicks));
			nextShapeClicks.clear();
			parent->shapeStackChangeListeners.invoke();
			nextShapeChangeListeners.invoke();
		}
	});
	signal_delete_event().connect([this](GdkEventAny*)
	{
		parent->shapeConfigOpened = false;
		parent->shapeConfigOpenedChangeListeners.invoke();
		return false;
	});
}

void ShapeConfigWindow::connectStateEventHandlers()
{
	parent->shapeStackChangeListeners.plug([this]
	{
		updateGeneralSensitivity();
	});
	parent->paramsOverwriteListeners.plug([this]
	{
		nextShapeClicks.clear();
		addShapeMode = AddShapeModeDefault;
		shapeSelector.set_active(AddShapeModeDefault);
		nextShapeChangeListeners.invoke();
		updateGeneralSensitivity();
		setEntryFields();
	});
	parent->inputValidators.plug([this]
	{
		return widthEntry.hasValidInput() && heightEntry.hasValidInput();
	});
	nextShapeChangeListeners.plug([this]
	{
		updateAddShapeWidgets();
	});
}

}