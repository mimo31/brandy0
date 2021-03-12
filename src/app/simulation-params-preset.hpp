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

	static constexpr uint32_t DefaultWp = 64, MinWp = 4, MaxWp = 16384, DefaultHp = 64, MinHp = 4, MaxHp = 16384,
					DefaultStepsPerFrame = 5, MaxStepsPerFrame = 1048576,
					DefaultFrameCapacity = 4096, MinFrameCapacity = 16, MaxFrameCapacity = 16777216;
	static constexpr double DefaultRho = 1.0, MinRho = 1e-5, MaxRho = 1e5,
					DefaultMu = 1.0, MinMu = 1e-5, MaxMu = 1e5,
					DefaultW = 1.0, MinW = 1e-4, MaxW = 1e4,
					DefaultH = 1.0, MinH = 1e-4, MaxH = 1e4,
					DefaultStopAfter = -1.0,//3e-3,
					DefaultDt = 1e-5, MinDt = 1e-9, MaxDt = 1e1,
					DefaultU = 0, MaxU = 1e3, // Min is -Max
					DefaultP = 0, MaxP = 1e5; // Min is -Max
	static constexpr BoundaryCondType DefaultVelocityType = BoundaryCondType::Dirichlet;
	static constexpr BoundaryCondType DefaultPressureType = BoundaryCondType::Neumann;

	static const BoundaryCond defaultBc;
	static const SimulationParams defaultParams;

	static const vec<SimulationParamsPreset> presets;

};

}

#endif // SIMULATION_PARAMS_PRESET_HPP