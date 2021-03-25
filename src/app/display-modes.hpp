/**
 * diplay-modes.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/13
 */
#ifndef DISPLAY_MODES_HPP
#define DISPLAY_MODES_HPP

#include <array>

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

const std::array<BackDisplayMode, 6> BackDisplayModes{ 
	BackDisplayMode("none"), BackDisplayMode("velocity magnitude"),
	BackDisplayMode("vorticity"), BackDisplayMode("relative vorticity"),
    BackDisplayMode("pressure"),
	BackDisplayMode("velocity divergence")
	};
constexpr uint32_t BackDisplayNone = 0;
constexpr uint32_t BackDisplayVelocityMagnitude = 1;
constexpr uint32_t BackDisplayVelocityCurl = 2;
constexpr uint32_t BackDisplayVelocityRelativeCurl = 3;
constexpr uint32_t BackDisplayPressure = 4;
constexpr uint32_t BackDisplayVelocityDiv = 5;
constexpr uint32_t BackDisplayModeDefault = BackDisplayVelocityMagnitude;

struct FrontDisplayMode
{
	str name;

	FrontDisplayMode(const str& name) : name(name)
	{
	}
};

const std::array<FrontDisplayMode, 3> FrontDisplayModes{
    FrontDisplayMode("none"), FrontDisplayMode("velocity (arrows)"),
    FrontDisplayMode("velocity (streamlines)")
};
constexpr uint32_t FrontDisplayNone = 0;
constexpr uint32_t FrontDisplayVelocityArrows = 1;
constexpr uint32_t FrontDisplayVelocityStreamlines = 2;
constexpr uint32_t FrontDisplayModeDefault = FrontDisplayVelocityArrows;

}

#endif // DISPLAY_MODES_HPP