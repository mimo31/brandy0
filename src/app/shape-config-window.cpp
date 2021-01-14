/**
 * shape-config-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/14
 */
#include "shape-config-window.hpp"

namespace brandy0
{

ShapeConfigWindow::ShapeConfigWindow()
    : BrandyWindow(1280, 720),
    widthEntry("width"),
    heightEntry("height"),
    undoButton("undo"),
    redoButton("redo"),
	polygonDoneButton("done")
{
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
}

}