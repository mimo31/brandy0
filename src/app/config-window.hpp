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
#include <gtkmm/window.h>

#include "glob.hpp"

#include "annotated-entry.hpp"
#include "simulator-params.hpp"

namespace brandy0
{

class ConfigWindow : public Gtk::Window
{
private:
	Gtk::Label descriptionLabel;

	Gtk::Button backHomeButton;
	Gtk::Button startSimButton;

	AnnotatedEntry rhoEntry, muEntry;

	AnnotatedEntry gridWidthEntry, gridHeightEntry, dtEntry, stepsPerFrameEntry, frameCapacityEntry;

	Gtk::CheckButton autoStop;

	Gtk::Grid rootGrid;

	Gtk::Frame compFrame;
	Gtk::Grid compGrid;

	Gtk::Frame physFrame;
	Gtk::Grid physGrid;

	SimulatorParams* params;

	void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal);
	void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal);
	void updateSubmitSensitivity();
public:
	ConfigWindow(const std::function<void()>& backHomeCallback);
	~ConfigWindow() override;
	void setParamsLocation(SimulatorParams *const params);
	void setEntryFields();
};

}

#endif // CONFIG_WINDOW_HPP
