/**
 * shape-config-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "shape-config-window.hpp"

#include "conv-utils.hpp"

namespace brandy0
{

void ShapeConfigWindow::updateUndoRedoSensitivity()
{
	undoButton.set_sensitive(parent->params->shapeStack.canUndo());
	redoButton.set_sensitive(parent->params->shapeStack.canRedo());
}

void ShapeConfigWindow::setEntryFields()
{
	std::unique_ptr<SimulatorParams>& params = parent->params;
	widthEntry.setText(std::to_string(params->w));
	heightEntry.setText(std::to_string(params->h));
}

ShapeConfigWindow::ShapeConfigWindow(ConfigStateAbstr *parent)
    : BrandyWindow(1280, 720),
    widthEntry("width"),
    heightEntry("height"),
    undoButton("undo"),
    redoButton("redo"),
	polygonDoneButton("done"),
	shapeWidget(parent),
	parent(parent)
{
	widthEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(widthEntry, parent->params->w, SimulatorParams::DEFAULT_W, SimulatorParams::MIN_W, SimulatorParams::MAX_W);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	heightEntry.hookInputHandler([this, parent]()
			{
			ConvUtils::updatePosRealIndicator(heightEntry, parent->params->h, SimulatorParams::DEFAULT_H, SimulatorParams::MIN_H, SimulatorParams::MAX_H);
			parent->validityChangeListeners.invoke();
			parent->dimensionsChangeListeners.invoke();
			}
			);
	
	undoButton.signal_clicked().connect([parent](){
		parent->params->shapeStack.undo();
		parent->shapeStackChangeListeners.invoke();
	});
	redoButton.signal_clicked().connect([parent](){
		parent->params->shapeStack.redo();
		parent->shapeStackChangeListeners.invoke();
	});
	
	parent->shapeStackChangeListeners.plug([this](){
		updateUndoRedoSensitivity();
	});
	parent->initListeners.plug([this](){
		updateUndoRedoSensitivity();
		setEntryFields();
	});
	parent->inputValidators.plug([this](){
		return widthEntry.hasValidInput() && heightEntry.hasValidInput();
	});

    widthEntry.attachTo(rootGrid, 0, 0);
    heightEntry.attachTo(rootGrid, 3, 0);

	polygonDoneButton.signal_clicked().connect([this](){ shapeWidget.submitCurrentPolygon(); });

    rootGrid.attach(undoButton, 6, 0);
    rootGrid.attach(redoButton, 7, 0);
	rootGrid.attach(polygonDoneButton, 8, 0);

    rootGrid.attach(shapeWidget, 0, 1, 9, 1);
	shapeWidget.show();
    
    add(rootGrid);

    show_all_children();
}

}