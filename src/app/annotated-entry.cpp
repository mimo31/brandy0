/**
 * annotated-entry.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/4
 */
#include "annotated-entry.hpp"

namespace brandy0
{

AnnotatedEntry::AnnotatedEntry(const str& label, StyleManager *const styleManager)
	: label(label), indicator(styleManager), disabled(false)
{
	this->label.set_xalign(0);
}

void AnnotatedEntry::attachTo(Gtk::Grid& grid, const uint32_t fromX, const uint32_t fromY)
{
	grid.attach(label, fromX, fromY);
	grid.attach(entry, fromX + 1, fromY);
	grid.attach(indicator, fromX + 2, fromY);
}

void AnnotatedEntry::hookInputHandler(const std::function<void()>& callback)
{
	entry.signal_changed().connect(callback);
}

str AnnotatedEntry::getText() const
{
	return entry.get_text();
}

void AnnotatedEntry::setText(const str& entryText)
{
	entry.set_text(entryText);
}

void AnnotatedEntry::indicateDefault()
{
	indicator.setDefault();
}

void AnnotatedEntry::indicateOk()
{
	indicator.setOk();
}

void AnnotatedEntry::indicateInvalid(const str& message)
{
	indicator.setInvalid(message);
}

bool AnnotatedEntry::hasValidInput() const
{
	const IndicatorState state = indicator.getState();
	return state == IndicatorState::DEFAULT || state == IndicatorState::OK;
}

void AnnotatedEntry::enable()
{
	if (disabled)
	{
		label.set_sensitive(true);
		entry.set_sensitive(true);
		indicator.pseudoShow();
		disabled = false;
	}
}

void AnnotatedEntry::disable()
{
	if (!disabled)
	{
		label.set_sensitive(false);
		entry.set_sensitive(false);
		indicator.pseudoHide();
		disabled = true;
	}
}

}
