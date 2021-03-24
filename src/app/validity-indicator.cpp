/**
 * validity-indicator.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/4
 */
#include "validity-indicator.hpp"

#include "str.hpp"

namespace brandy0
{

const Gtk::IconSize iconSize = Gtk::BuiltinIconSize::ICON_SIZE_MENU;

ValidityIndicator::ValidityIndicator(StyleManager *const styleManager)
	: message("(default)"),
	state(IndicatorState::DEFAULT),
	styleManager(styleManager)
{
	set_valign(Gtk::ALIGN_CENTER);
	icon = new Gtk::Image("dialog-ok", iconSize);

	styleManager->requestInit();

	currentStyle = styleManager->darkGreenStyle;
	message.get_style_context()->add_provider(currentStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

	message.set_width_chars(30);
	message.set_xalign(0.0);

	set_column_spacing(5);

	attach(*icon, 0, 0, 1, 1);
	attach(message, 1, 0, 1, 1);
}

ValidityIndicator::~ValidityIndicator()
{
	delete icon;
}

void ValidityIndicator::changeStyle(const Glib::RefPtr<Gtk::CssProvider> style)
{
	if (currentStyle != style)
	{
		message.get_style_context()->remove_provider(currentStyle);
		message.get_style_context()->add_provider(style, GTK_STYLE_PROVIDER_PRIORITY_USER);
		currentStyle = style;
	}
}

void ValidityIndicator::setOkIcon()
{
	delete icon;
	icon = new Gtk::Image("dialog-ok", iconSize);
	attach(*icon, 0, 0, 1, 1);
	icon->show();
}

void ValidityIndicator::setInvalidIcon()
{
	delete icon;
	icon = new Gtk::Image("dialog-close", iconSize);
	attach(*icon, 0, 0, 1, 1);
	icon->show();
}

void ValidityIndicator::setOk()
{
	if (state == IndicatorState::OK)
		return;
	message.set_text("ok");
	changeStyle(styleManager->greenStyle);
	if (state == IndicatorState::INVALID)
	{
		setOkIcon();
	}
	state = IndicatorState::OK;
}

void ValidityIndicator::setDefault()
{
	if (state == IndicatorState::DEFAULT)
		return;
	message.set_text("(default)");
	changeStyle(styleManager->darkGreenStyle);
	if (state == IndicatorState::INVALID)
	{
		setOkIcon();
	}
	state = IndicatorState::DEFAULT;
}

void ValidityIndicator::setInvalid(const str& message)
{
	this->message.set_text(message);
	if (state != IndicatorState::INVALID)
	{
		setInvalidIcon();
		changeStyle(styleManager->redStyle);
		state = IndicatorState::INVALID;
	}
}

IndicatorState ValidityIndicator::getState() const
{
	return state;
}

}
