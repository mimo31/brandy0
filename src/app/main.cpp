#include <iostream>
#include <cstdlib>

#include "simulator.hpp"

namespace brandy0
{

using std::cout;
using std::endl;

void run()
{
	// ---
	// mock
	cout << "brandy0 started" << endl;
	uint32_t w = 32, h = 32;
	Grid<bool> solid(w, h);
	solid.set_all(false);
	SimulatorParams params(1, 1, w, h, .001,
			BoundaryCond(vec2d(1, 0), PressureBoundaryCond::DIRICHLET),
			BoundaryCond(vec2d(1, 0), PressureBoundaryCond::DIRICHLET),
			BoundaryCond(vec2d(0, 0), PressureBoundaryCond::VON_NEUMANN),
			BoundaryCond(vec2d(0, 0), PressureBoundaryCond::VON_NEUMANN),
			1, .1, solid);
	Simulator s(params);
	s.iter();
	// ---
}

}

int main()
{
	brandy0::run();
	return EXIT_SUCCESS;
}
