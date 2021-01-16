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
	undoButton.set_sensitive(shapeWidget.canUndo());
	redoButton.set_sensitive(shapeWidget.canRedo());
}

ShapeConfigWindow::ShapeConfigWindow(const std::function<void(bool)>& scalarsValidityChangedCallback)
    : BrandyWindow(1280, 720),
    widthEntry("width"),
    heightEntry("height"),
    undoButton("undo"),
    redoButton("redo"),
	polygonDoneButton("done"),
	shapeWidget([this](){ updateUndoRedoSensitivity(); })
{
	widthEntry.hookInputHandler([this, scalarsValidityChangedCallback]()
			{
			ConvUtils::updatePosRealIndicator(widthEntry, params->w, SimulatorParams::DEFAULT_W, SimulatorParams::MIN_W, SimulatorParams::MAX_W);
			scalarsValidityChangedCallback(widthEntry.hasValidInput() && heightEntry.hasValidInput());
			if (widthEntry.hasValidInput())
			{
				shapeWidget.setW(params->w);
				shapeWidget.refresh();
			}
			}
			);
	heightEntry.hookInputHandler([this, scalarsValidityChangedCallback]()
			{
			ConvUtils::updatePosRealIndicator(heightEntry, params->h, SimulatorParams::DEFAULT_H, SimulatorParams::MIN_H, SimulatorParams::MAX_H);
			scalarsValidityChangedCallback(widthEntry.hasValidInput() && heightEntry.hasValidInput());
			if (heightEntry.hasValidInput())
			{
				shapeWidget.setH(params->h);
				shapeWidget.refresh();
			}
			}
			);

    widthEntry.attachTo(rootGrid, 0, 0);
    heightEntry.attachTo(rootGrid, 3, 0);

	polygonDoneButton.signal_clicked().connect([this](){ shapeWidget.submitCurrentPolygon(); shapeWidget.refresh(); });

    rootGrid.attach(undoButton, 6, 0);
    rootGrid.attach(redoButton, 7, 0);
	rootGrid.attach(polygonDoneButton, 8, 0);

    rootGrid.attach(shapeWidget, 0, 1, 9, 1);
	shapeWidget.show();
    
    add(rootGrid);

    show_all_children();

	updateUndoRedoSensitivity();
}

void ShapeConfigWindow::setParamsLocation(SimulatorParams *const params)
{
	this->params = params;
}

void ShapeConfigWindow::setFromParams()
{
	widthEntry.setText(std::to_string(params->w));
	heightEntry.setText(std::to_string(params->h));
	shapeWidget.setW(params->w);
	shapeWidget.setH(params->h);
	shapeWidget.setWp(params->wp);
	shapeWidget.setHp(params->hp);
}

void ShapeConfigWindow::writeObstaclesToParams()
{
	shapeWidget.writeShapeStack(params);
}

void ShapeConfigWindow::refreshGridSize()
{
	shapeWidget.setWp(params->wp);
	shapeWidget.setHp(params->hp);
	shapeWidget.refresh();
}

}