/**
 * simulation-params-preset.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#ifndef SIMULATION_PARAMS_PRESET_HPP
#define SIMULATION_PARAMS_PRESET_HPP

#include "simulation-params.hpp"
#include "str.hpp"
#include "vec.hpp"

namespace brandy0
{

class SimulationParamsPreset
{
public:
	SimulationParams params;
	str name;

	SimulationParamsPreset(const SimulationParams &params, const str &name)
		: params(params), name(name)
	{
	}

	static constexpr uint32_t DEFAULT_WP = 64, MAX_WP = 16384, DEFAULT_HP = 64, MAX_HP = 16384,
					DEFAULT_STEPS_PER_FRAME = 5, MAX_STEPS_PER_FRAME = 1048576, DEFAULT_FRAME_CAPACITY = 4096, MAX_FRAME_CAPACITY = 16777216;
	static constexpr double DEFAULT_RHO = 1.0, MIN_RHO = 1e-5, MAX_RHO = 1e5,
					DEFAULT_MU = 1.0, MIN_MU = 1e-5, MAX_MU = 1e5,
					DEFAULT_W = 1.0, MIN_W = 1e-4, MAX_W = 1e4,
					DEFAULT_H = 1.0, MIN_H = 1e-4, MAX_H = 1e4,
					DEFAULT_STOP_AFTER = -1.0,//3e-3,
					DEFAULT_DT = 1e-5, MIN_DT = 1e-9, MAX_DT = 1e1,
					DEFAULT_U = 0, MAX_U = 1e3; // MIN is -MAX
	static constexpr PressureBoundaryCond DEFAULT_PRESSURE_BC = PressureBoundaryCond::NEUMANN;

	static const BoundaryCond defaultBc;
	static const SimulationParams defaultParams;

	static const vec<SimulationParamsPreset> presets;

};

}

#endif // SIMULATION_PARAMS_PRESET_HPP