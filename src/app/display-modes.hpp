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

/**
 * Struct representing one possible background visual mode.
 * The background visual modes are primarily identified
 * by their index in the global const array of all background visual modes.
 */
struct BackDisplayMode
{
	/// Name of the mode
	str name;

	/**
	 * Constructs a background visual mode object
	 * @param name name of the mode
	 */
	BackDisplayMode(const str& name) : name(name)
	{
	}
};

/// Array of all background visual modes available in our program
const std::array<BackDisplayMode, 6> BackDisplayModes{ 
	BackDisplayMode("none"), BackDisplayMode("velocity magnitude"),
	BackDisplayMode("vorticity"), BackDisplayMode("relative vorticity"),
	BackDisplayMode("pressure"),
	BackDisplayMode("velocity divergence")
	};
/// Number (index) of the background visual mode 'None'
constexpr uint32_t BackDisplayNone = 0;
/// Number (index) of the background visual mode 'velocity magnitude'
constexpr uint32_t BackDisplayVelocityMagnitude = 1;
/// Number (index) of the background visual mode 'vorticity'
constexpr uint32_t BackDisplayVelocityCurl = 2;
/// Number (index) of the background visual mode 'relative vorticity'
constexpr uint32_t BackDisplayVelocityRelativeCurl = 3;
/// Number (index) of the background visual mode 'pressure'
constexpr uint32_t BackDisplayPressure = 4;
/// Number (index) of the background visual mode 'velocity divergence'
constexpr uint32_t BackDisplayVelocityDiv = 5;
/// Number (index) of the default background visual mode
constexpr uint32_t BackDisplayModeDefault = BackDisplayVelocityMagnitude;

/**
 * Struct representing one possible foreground visual mode.
 * The foreground visual modes are primarily identified
 * by their index in the global const array of all foreground visual modes.
 */
struct FrontDisplayMode
{
	/// Name of the mode
	str name;

	/**
	 * Constructs a foreground visual mode object
	 * @param name name of the mode
	 */
	FrontDisplayMode(const str& name) : name(name)
	{
	}
};

/// Array of all foreground visual modes available in our program
const std::array<FrontDisplayMode, 3> FrontDisplayModes{
	FrontDisplayMode("none"), FrontDisplayMode("velocity (arrows)"),
	FrontDisplayMode("velocity (streamlines)")
};
/// Number (index) of the foreground visual mode 'None'
constexpr uint32_t FrontDisplayNone = 0;
/// Number (index) of the foreground visual mode 'velocity (arrows)'
constexpr uint32_t FrontDisplayVelocityArrows = 1;
/// Number (index) of the foreground visual mode 'velocity (streamlines)'
constexpr uint32_t FrontDisplayVelocityStreamlines = 2;
/// Number (index) of the default foreground visual mode
constexpr uint32_t FrontDisplayModeDefault = FrontDisplayVelocityArrows;

}

#endif // DISPLAY_MODES_HPP