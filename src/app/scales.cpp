/**
 * scales.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/24
 */
#include "scales.hpp"

namespace brandy0
{

SpeedScale::SpeedScale()
{
	set_draw_value(false);
	set_range(-1, 1);
	set_has_origin(false);
	set_increments(.001, .001);
}

double SpeedScale::getSpeedup() const
{
	return exp(log(MaxSpeedup) * get_value());
}

void MutableScale::quietSetValue(const double value)
{
	settingValue = true;
	set_value(value);
	settingValue = false;
}

void MutableScale::connectUserChangedHandler(const VoidFunc &f)
{
	signal_value_changed().connect([this, f]
	{
		if (!settingValue)
			f();
	});
}

}