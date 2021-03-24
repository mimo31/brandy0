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
private:
	Grid<vec2d> w;
    Grid<double> field;
	Grid<bool> dirichlet;
	Grid<bool> visited;

	void visit(Point p);

    double lapL1limit;
	double crashLimit;

    void enforcePBoundary(Grid<double>& p);
    void enforceUBoundary(Grid<vec2d>& u);
    void enforceBoundary(SimFrame& f);
public:
	SimulatorClassic(const SimulationParams& params);
    void iter() override;
};

}


#endif // SIMULATOR_CLASSIC_HPP