/**
 * simulator-classic.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/26
 */
#ifndef SIMULATOR_CLASSIC_HPP
#define SIMULATOR_CLASSIC_HPP

#include "glob.hpp"

#include "simulator.hpp"

namespace brandy0
{

class SimulatorClassic : public Simulator
{
public:
	SimulatorClassic(const SimulatorParams& params);
    void iter();
};

}


#endif // SIMULATOR_CLASSIC_HPP