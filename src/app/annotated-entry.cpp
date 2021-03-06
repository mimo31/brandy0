/**
 * annotated-entry.cpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/4
 */
#include "annotated-entry.hpp"

namespace brandy0
{

AnnotatedEntry::AnnotatedEntry(const std::string& label)
	: label(label)
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

std::string AnnotatedEntry::getText() const
{
	return entry.get_text();
}

void AnnotatedEntry::setText(const std::string& entryText)
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

void AnnotatedEntry::indicateInvalid(const std::string& message)
{
	indicator.setInvalid(message);
}

bool AnnotatedEntry::hasValidInput() const
{
	const IndicatorState state = indicator.getState();
	return state == IndicatorState::DEFAULT || state == IndicatorState::OK;
}

}
