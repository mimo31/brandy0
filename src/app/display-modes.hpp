/**
 * diplay-modes.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/13
 */
#ifndef DISPLAY_MODES_HPP
#define DISPLAY_MODES_HPP

#include "glob.hpp"

#include "str.hpp"

namespace brandy0
{

struct BackDisplayMode
{
	str name;

	BackDisplayMode(const str& name) : name(name)
	{
	}
};

constexpr uint32_t BACK_DISPLAY_MODE_COUNT = 6;
const BackDisplayMode BACK_DISPLAY_MODES[BACK_DISPLAY_MODE_COUNT] = { 
	BackDisplayMode("none"), BackDisplayMode("velocity magnitude"),
	BackDisplayMode("vorticity"), BackDisplayMode("relative vorticity"),
    BackDisplayMode("pressure"),
	BackDisplayMode("velocity divergence")
	};
constexpr uint32_t BACK_DISPLAY_NONE = 0;
constexpr uint32_t BACK_DISPLAY_VELOCITY_MAGNITUDE = 1;
constexpr uint32_t BACK_DISPLAY_VELOCITY_CURL = 2;
constexpr uint32_t BACK_DISPLAY_VELOCITY_RELATIVE_CURL = 3;
constexpr uint32_t BACK_DISPLAY_PRESSURE = 4;
constexpr uint32_t BACK_DISPLAY_VELOCITY_DIV = 5;
constexpr uint32_t BACK_DISPLAY_MODE_DEFAULT = BACK_DISPLAY_VELOCITY_MAGNITUDE;

struct FrontDisplayMode
{
	str name;

	FrontDisplayMode(const str& name) : name(name)
	{
	}
};

constexpr uint32_t FRONT_DISPLAY_MODE_COUNT = 3;
const FrontDisplayMode FRONT_DISPLAY_MODES[FRONT_DISPLAY_MODE_COUNT] = {
    FrontDisplayMode("none"), FrontDisplayMode("velocity (arrows)"),
    FrontDisplayMode("velocity (streamlines)")
};
constexpr uint32_t FRONT_DISPLAY_NONE = 0;
constexpr uint32_t FRONT_DISPLAY_VELOCITY_ARROWS = 1;
constexpr uint32_t FRONT_DISPLAY_VELOCITY_STREAMLINES = 2;
constexpr uint32_t FRONT_DISPLAY_MODE_DEFAULT = FRONT_DISPLAY_VELOCITY_ARROWS;

}

#endif // DISPLAY_MODES_HPP