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

class SpeedScale : public Gtk::Scale
{
public:
	static constexpr double MaxSpeedup = 10000;
	SpeedScale();
	double getSpeedup() const;
};

class MutableScale : public Gtk::Scale
{
private:
	bool settingValue = false;
public:
	void quietSetValue(double value);
	void connectUserChangedHandler(const VoidFunc &f);
};

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