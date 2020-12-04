/**
 * annotated-entry.hpp
 *
 * Author: Viktor Fukala
 * Creted on 2020/12/4
 */
#ifndef ANNOTATED_ENTRY_HPP
#define ANNOTATED_ENTRY_HPP

#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "glob.hpp"

#include "validity-indicator.hpp"

namespace brandy0
{

class AnnotatedEntry
{
private:
	Gtk::Label label;
	Gtk::Entry entry;
	ValidityIndicator indicator;
public:
	AnnotatedEntry(const std::string& label);
	void attachTo(Gtk::Grid&, const uint32_t fromX, const uint32_t fromY);
	void hookInputHandler(const std::function<void()>&);
	std::string getText();
	void setText(const std::string& entryText);
	void indicateDefault();
	void indicateOk();
	void indicateInvalid(const std::string& message);
	bool hasValidInput() const;
};

}

#endif // ANNOTATED_ENTRY_HPP
