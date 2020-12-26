/**
 * simulator-classic.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/26
 */
#include "simulator-classic.hpp"

namespace brandy0
{

SimulatorClassic::SimulatorClassic(const SimulatorParams& params)
    : Simulator(params)
{   
}

void SimulatorClassic::iter()
{
	for (uint32_t y = 0; y < hp; y++)
	{
		for (uint32_t x = 0; x < wp; x++)
		{
			const vec2d coor = to_coor(x, y);
			f1.u(x, y) = (coor - vec2d(w / 2, h / 2)).get_lrot();
		}
	}
}

}