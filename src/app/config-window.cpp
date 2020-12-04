/**
 * config-window.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/3
 */
#include "config-window.hpp"

#include <gtkmm/cssprovider.h>

namespace brandy0
{

ConfigWindow::ConfigWindow(const std::function<void()>& backHomeCallback)
	: descriptionLabel("Configure your simulation."),
	compFrame("computation configuration"),
	physFrame("physics configuration"),
	backHomeButton("back to home"),
	startSimButton("start simulation"),
	rhoEntry("rho (density):"),
	muEntry("mu (viscosity):"),
	gridWidthEntry("grid width:"),
	gridHeightEntry("grid height:"),
	dtEntry("dt (time step):"),
	stepsPerFrameEntry("steps per frame:"),
	frameCapacityEntry("frame capacity:")
{
	set_title("brandy0");
	set_border_width(12);

	backHomeButton.signal_clicked().connect(backHomeCallback);

	rhoEntry.hookInputHandler([this]()
			{
			updatePosRealIndicator(rhoEntry, params->rho, SimulatorParams::DEFAULT_RHO, SimulatorParams::MIN_RHO, SimulatorParams::MAX_RHO);
			}
			);
	muEntry.hookInputHandler([this]()
			{
			updatePosRealIndicator(muEntry, params->mu, SimulatorParams::DEFAULT_MU, SimulatorParams::MIN_MU, SimulatorParams::MAX_MU);
			}
			);

	gridWidthEntry.hookInputHandler([this]()
			{
			updatePosIntIndicator(gridWidthEntry, params->wp, SimulatorParams::DEFAULT_WP, SimulatorParams::MAX_WP);
			}
			);
	gridHeightEntry.hookInputHandler([this]()
			{
			updatePosIntIndicator(gridHeightEntry, params->hp, SimulatorParams::DEFAULT_HP, SimulatorParams::MAX_HP);
			}
			);
	stepsPerFrameEntry.hookInputHandler([this]()
			{
			updatePosIntIndicator(stepsPerFrameEntry, params->stepsPerFrame, SimulatorParams::DEFAULT_STEPS_PER_FRAME, SimulatorParams::MAX_STEPS_PER_FRAME);
			}
			);
	frameCapacityEntry.hookInputHandler([this]()
			{
			updatePosIntIndicator(frameCapacityEntry, params->frameCapacity, SimulatorParams::DEFAULT_FRAME_CAPACITY, SimulatorParams::MAX_FRAME_CAPACITY);
			}
			);

	dtEntry.hookInputHandler([this]()
			{
			updatePosRealIndicator(dtEntry, params->dt, SimulatorParams::DEFAULT_DT, SimulatorParams::MIN_DT, SimulatorParams::MAX_DT);
			}
			);

	Glib::RefPtr<Gtk::CssProvider> framePaddingStyle = Gtk::CssProvider::create();
	framePaddingStyle->load_from_data("grid { padding: 5px }");

	physGrid.get_style_context()->add_provider(framePaddingStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);
	compGrid.get_style_context()->add_provider(framePaddingStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	physGrid.set_column_spacing(5);
	compGrid.set_column_spacing(5);

	rhoEntry.attachTo(physGrid, 0, 0);
	muEntry.attachTo(physGrid, 0, 1);

	physFrame.add(physGrid);

	gridWidthEntry.attachTo(compGrid, 0, 0);
	gridHeightEntry.attachTo(compGrid, 0, 1);
	dtEntry.attachTo(compGrid, 0, 2);
	stepsPerFrameEntry.attachTo(compGrid, 0, 3);
	frameCapacityEntry.attachTo(compGrid, 0, 4);
	
	compFrame.add(compGrid);

	rootGrid.attach(descriptionLabel, 0, 0, 2, 1);
	rootGrid.attach(physFrame, 0, 1, 1, 1);
	rootGrid.attach(compFrame, 1, 1, 1, 1);
	rootGrid.attach(backHomeButton, 0, 2, 1, 1);
	rootGrid.attach(startSimButton, 1, 2, 1, 1);

	add(rootGrid);

	show_all_children();
}

bool isPositiveReal(const std::string& s)
{
	uint32_t ind = 0;
	while (ind < s.length() && s[ind] != 'e' && s[ind] != 'E')
		ind++;
	if (ind == 0 || ind == s.length() - 1)
		return false;
	uint32_t dotind = UINT32_MAX;
	for (uint32_t i = 0; i < ind; i++)
	{
		if (s[i] == '.')
		{
			dotind = i;
			break;
		}
	}
	if (dotind == ind - 1)
		return false;
	if (dotind == UINT32_MAX)
		dotind = ind;
	for (uint32_t i = 0; i < dotind; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	for (uint32_t i = dotind + 1; i < ind; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	if (s[ind + 1] == '-' && ind + 1 == s.length() - 1)
		return false;
	for (uint32_t i = (s[ind + 1] == '-' ? ind + 1 : ind) + 1; i < s.length(); i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	return true;
}

bool isNonnegativeInt(const std::string& s)
{
	for (const char c : s)
	{
		if (c < '0' || c > '9')
			return false;
	}
	return true;
}

bool isNonzero(const std::string& s)
{
	for (const char c : s)
	{
		if (c != '0')
			return true;
	}
	return false;
}

bool boundedStoi(const std::string& s, uint32_t& writeto, const uint32_t maxVal)
{
	uint32_t val = 0;
	for (uint32_t i = 0; i < s.length(); i++)
	{
		val = val * 10 + s[i] - '0';
		if (val > maxVal)
			return false;
	}
	writeto = val;
	return true;
}

void ConfigWindow::updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal)
{
	const std::string entered = aentry.getText();
	if (entered == "" || !isPositiveReal(entered))
		aentry.indicateInvalid("enter a positive integer");
	else if (!boundedStoi(entered, writeto, maxVal))
		aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
	else
	{
		if (writeto == defaultVal)
			aentry.indicateDefault();
		else
			aentry.indicateOk();
	}
	updateSubmitSensitivity();
}

void ConfigWindow::updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal)
{
	const std::string entered = aentry.getText();
	if (entered == "" || !isPositiveReal(entered))
	{
		aentry.indicateInvalid("enter a positive real number");
	}
	else
	{
		const double val = std::stod(entered, nullptr);
		if (val == 0)
		{
			aentry.indicateInvalid("enter a positive real number");
		}
		else if (val < minVal)
		{
			aentry.indicateInvalid("allowed min. is " + std::to_string(minVal));
		}
		else if (val > maxVal)
		{
			aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
		}
		else
		{
			writeto = val;
			if (val == defaultVal)
				aentry.indicateDefault();
			else
				aentry.indicateOk();
		}
	}
	updateSubmitSensitivity();
}

void ConfigWindow::updateSubmitSensitivity()
{
	startSimButton.set_sensitive(
			rhoEntry.hasValidInput()
			&& muEntry.hasValidInput()
			&& gridWidthEntry.hasValidInput()
			&& gridHeightEntry.hasValidInput()
			&& dtEntry.hasValidInput()
			&& stepsPerFrameEntry.hasValidInput()
			&& frameCapacityEntry.hasValidInput());
}

void ConfigWindow::setParamsLocation(SimulatorParams *const params)
{
	this->params = params;
}

ConfigWindow::~ConfigWindow()
{
}

void ConfigWindow::setEntryFields()
{
	using std::to_string;
	gridWidthEntry.setText(to_string(params->wp));
	gridHeightEntry.setText(to_string(params->hp));
	rhoEntry.setText(to_string(params->rho));
	muEntry.setText(to_string(params->mu));
	dtEntry.setText(to_string(params->dt));
	stepsPerFrameEntry.setText(to_string(params->stepsPerFrame));
	frameCapacityEntry.setText(to_string(params->frameCapacity));
}

}
