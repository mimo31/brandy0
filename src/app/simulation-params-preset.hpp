/**
 * simulation-params-preset.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#ifndef SIMULATION_PARAMS_PRESET_HPP
#define SIMULATION_PARAMS_PRESET_HPP

#include <array>

#include "simulation-params.hpp"
#include "str.hpp"

namespace brandy0
{

/**
 * Class representing a predefined value of simulation parameters
 */
class SimulationParamsPreset
{
public:
	/// The predefined value of simulation parameters
	SimulationParams params;
	/// Name of the preset
	str name;

	/**
	 * Constructs a SimulationParamsPreset object
	 * @param params value of simulation parameters
	 * @param name name of the preset
	 */
	SimulationParamsPreset(const SimulationParams &params, const str &name)
		: params(params), name(name)
	{
	}

	// default, minimum, and maximum values of the simulation parameters

	/// Default grid width
	static constexpr uint32_t DefaultWp = 64;
	/// Minimum grid width
	static constexpr uint32_t MinWp = 4;
	/// Maximum grid width
	static constexpr uint32_t MaxWp = 16384;
	/// Default grid height
	static constexpr uint32_t DefaultHp = 64;
	/// Minimum grid height
	static constexpr uint32_t MinHp = 4;
	/// Maximum grid height
	static constexpr uint32_t MaxHp = 16384;
	/// Default number of steps per frame
	static constexpr uint32_t DefaultStepsPerFrame = 5;
	/// Maximum number of steps per frame
	static constexpr uint32_t MaxStepsPerFrame = 1048576;
	/// Default capacity for computed frames
	static constexpr uint32_t DefaultFrameCapacity = 4096;
	/// Minimum capacity for computed frames
	static constexpr uint32_t MinFrameCapacity = 16;
	/// Maximum capacity for computed frames
	static constexpr uint32_t MaxFrameCapacity = 16777216;
	/// Default density
	static constexpr double DefaultRho = 1.0;
	/// Minimum density
	static constexpr double MinRho = 1e-5;
	/// Maximum density
	static constexpr double MaxRho = 1e5;
	/// Default viscosity
	static constexpr double DefaultMu = 1.0;
	/// Minimum viscosity
	static constexpr double MinMu = 1e-5;
	/// Maximum viscosity
	static constexpr double MaxMu = 1e5;
	/// Default physical width
	static constexpr double DefaultW = 1.0;
	/// Minimum physical width
	static constexpr double MinW = 1e-4;
	/// Maximum physical width
	static constexpr double MaxW = 1e4;
	/// Default physical height
	static constexpr double DefaultH = 1.0;
	/// Minimum physical height
	static constexpr double MinH = 1e-4;
	/// Maximum physical height
	static constexpr double MaxH = 1e4;
	/// Default time for automatic computation pause
	static constexpr double DefaultStopAfter = -1.0;
	/// Default dt (time step)
	static constexpr double DefaultDt = 1e-5;
	/// Minimum dt (time step)
	static constexpr double MinDt = 1e-9;
	/// Maximum dt (time step)
	static constexpr double MaxDt = 1e1;
	/// Default value of each component in a Dirichlet boundary condition for velocity
	static constexpr double DefaultU = 0;
	/// Maximum absolute value of each component in a Dirichlet boundary condition for velocity
	static constexpr double MaxU = 1e3;
	/// Default value in a Dirichlet boundary condition for pressure
	static constexpr double DefaultP = 0;
	/// Maximum absolute value in a Dirichlet boundary condition for pressure
	static constexpr double MaxP = 1e5;
	/// Default type of boundary condition for velocity
	static constexpr BoundaryCondType DefaultVelocityType = BoundaryCondType::Dirichlet;
	/// Default type of boundary condition for pressure
	static constexpr BoundaryCondType DefaultPressureType = BoundaryCondType::Neumann;

	/// Default boundary condition (consisting of the default values of all its components)
	static const BoundaryCond DefaultBc;
	/// Default simulation parameters (consisting of the default values of all its components)
	static const SimulationParams DefaultParams;

	/// List of all simulation parameters presets
	static const std::array<SimulationParamsPreset, 13> Presets;

};

}

#endif // SIMULATION_PARAMS_PRESET_HPP