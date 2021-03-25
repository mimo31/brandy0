/**
 * scales.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/24
 */
#ifndef SCALES_HPP
#define SCALES_HPP

#include <gtkmm/scale.h>

namespace brandy0
{

class SpeedScale : public Gtk::Scale
{
public:
	static constexpr double MaxSpeedup = 10000;
	SpeedScale();
	double getSpeedup() const;
};

class TimeScale : public Gtk::Scale
{
public:
	TimeScale();
};

}

#endif // SCALES_HPP