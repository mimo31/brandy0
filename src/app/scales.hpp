/**
 * scales.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/24
 */
#ifndef SCALES_HPP
#define SCALES_HPP

#include <gtkmm/scale.h>

#include "func.hpp"

namespace brandy0
{

/**
 * Class representing a Gtk::Scale used for setting simulation playback speed
 */
class SpeedScale : public Gtk::Scale
{
public:
	/// Maximal possible speedup (attained when scale value is maximized). Minimal possible speedup (slowdown) is 1 / MaxSpeedup
	static constexpr double MaxSpeedup = 10000;
	/**
	 * Constructs a SpeedScale object. Default constructs the Gtk::Scale base and sets some parameters (e.g. step size)
	 */
	SpeedScale();
	/**
	 * @return playback speedup currently set in this scale (proportional to the logarithm of the scale's value)
	 */
	double getSpeedup() const;
};

/**
 * Thin wrapper for Gtk::Scale allowing to listen to a value changed event only if fired by the value being changed by the user.
 * Used for time scales in which setting the value starts a value editing mode and that should not happen for the value being changed automatically.
 */
class MutableScale : public Gtk::Scale
{
private:
	/**
	 * True iff the scale's value is currently being set by quietSetValue, so that the listeners waiting for a change in value by the user should not be fired
	 * @see quietSetValue
	 * @see connectUserChangedHandler
	 */
	bool settingValue = false;
public:
	/**
	 * Sets the scale's value without firing listeners registered though connectUserChangedHandler
	 * @see connectUserChangedHandler
	 */
	void quietSetValue(double value);
	/**
	 * Connects a listerner for the event of the scale's value being changed by the application's user
	 * that will not be called when the scale's value is changed by quietSetValue
	 * @see quietSetValue
	 */
	void connectUserChangedHandler(const VoidFunc &f);
};

/**
 * Templated class that represents a Gtk::Scale used for setting and displaying playback time.
 * T should derive from Gtk::Scale
 */
template<typename T>
class TimeScaleTemplate : public T
{
public:
	TimeScaleTemplate()
	{
		T::set_draw_value(false);
		T::set_range(0, 1);
		T::set_increments(.001, .001);
	}
};

typedef TimeScaleTemplate<Gtk::Scale> TimeScale;
typedef TimeScaleTemplate<MutableScale> MutableTimeScale;

}

#endif // SCALES_HPP