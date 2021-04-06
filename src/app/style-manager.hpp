/**
 * style-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/20
 */
#ifndef STYLE_MANAGER_HPP
#define STYLE_MANAGER_HPP

#include <gtkmm/cssprovider.h>
#include <gtkmm/grid.h>

namespace brandy0
{

/**
 * Class managing some aspects of the style of the application's windows
 */
class StyleManager
{
	/// True iff this StyleManager object has already been initialized
	bool initialized = false;
public:
	/// Red text style for labels (only when this object already initialized)
	Glib::RefPtr<Gtk::CssProvider> redStyle;
	/// Green text style for labels (only when this object already initialized)
	Glib::RefPtr<Gtk::CssProvider> greenStyle;
	/// Blue text style for labels (only when this object already initialized)
	Glib::RefPtr<Gtk::CssProvider> darkGreenStyle;

	/**
	 * Initializes this StyleManager.
	 * Does nothing if alread initialized.
	 */
	void requestInit();

	/// Default recommended vertical and horizontal spacing between widgets in grid
	static constexpr uint32_t WidgetSpacing = 5;
	
	/**
	 * Sets the recommended spacing around a grid widget
	 * @param grid grid to set the padding to
	 */
	static void setPadding(Gtk::Grid &grid);
};

}

#endif // STYLE_MANAGER_HPP