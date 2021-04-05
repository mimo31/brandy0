/**
 * sim-frame.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef SIM_FRAME_HPP
#define SIM_FRAME_HPP

#include "grid.hpp"
#include "vec2d.hpp"

namespace brandy0
{

/**
 * Class representing one computed frame of the simulation.
 * Includes no direct information about the parameters of the simulation, only the computed values of the fields.
 */
class SimFrame
{
public:
	/// Pressure field
	Grid<double> p;
	/// Velocity field
	Grid<vec2d> u;

	/**
	 * Constructs a SimFrame object
	 * @param p grid with the values of the pressure field
	 * @param u grid with the values of the velocity field
	 */
	SimFrame(const Grid<double>& p, const Grid<vec2d>& u)
		: p(p), u(u)
	{
	}
};

}

#endif // SIM_FRAME_HPP
