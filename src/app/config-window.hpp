/**
 * config-window.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#ifndef CONFIG_WINDOW_HPP
#define CONFIG_WINDOW_HPP

#include <functional>

#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include "glob.hpp"

#include "annotated-entry.hpp"
#include "bc-selector.hpp"
#include "brandy-window.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class ConfigWindow : public BrandyWindow
{
private:
	Gtk::Label descriptionLabel;

	AnnotatedEntry rhoEntry, muEntry;
	BCSelector x0sel, x1sel, y0sel, y1sel;

	AnnotatedEntry gridWidthEntry, gridHeightEntry, dtEntry, stepsPerFrameEntry, frameCapacityEntry;

	Gtk::CheckButton autoStop;

	Gtk::Grid rootGrid;

	Gtk::Frame physFrame;
	Gtk::Grid physGrid;

	Gtk::Frame compFrame;
	Gtk::Grid compGrid;

	Gtk::Button backHomeButton;
	Gtk::Button startSimButton;

	SimulatorParams* params;

	void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal);
	void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal);
	void updateSubmitSensitivity();
public:
	ConfigWindow(const std::function<void()>& backHomeCallback, const std::function<void()>& startSimulationCallback);
	~ConfigWindow() override;
	void setParamsLocation(SimulatorParams *const params);
	void setEntryFields();
};

}

#endif // CONFIG_WINDOW_HPP
