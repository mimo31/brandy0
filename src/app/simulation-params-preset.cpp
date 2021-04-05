/**
 * simulation-params-preset.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#include "simulation-params-preset.hpp"

namespace brandy0
{

const BoundaryCond SimulationParamsPreset::DefaultBc(DefaultVelocityType, vec2d(DefaultU, DefaultU), DefaultPressureType, DefaultP);

const SimulationParams SimulationParamsPreset::DefaultParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, DefaultBc, DefaultBc, DefaultBc, DefaultBc, DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity);

const std::array<SimulationParamsPreset, 13> SimulationParamsPreset::Presets {
	SimulationParamsPreset(SimulationParamsPreset::DefaultParams, "static (default)"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Dirichlet, vec2d(0, .1), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultBc,
				DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, high visc."),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 128, 128,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Dirichlet, vec2d(0, .1), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultBc,
				DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, high visc., 128x128"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 256, 256,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Dirichlet, vec2d(0, .1), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultBc,
				DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, high visc., 256x256"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Dirichlet, vec2d(0, .1), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultBc,
				DefaultRho, .03, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, low visc."),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultRho, 1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .3), vec2d(.7, .7)) } ),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "square obs., high visc."),
				
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultRho, 1e-3,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .3), vec2d(.7, .7)) } ),
				DefaultStopAfter, 50, DefaultFrameCapacity), "square obs., low visc."),
				
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 128, 128,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultRho, 1e-3,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .3), vec2d(.7, .7)) } ),
				DefaultStopAfter, 50, DefaultFrameCapacity), "square obs., low visc., 128x128"),
				
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 256, 256,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultBc, DefaultBc, DefaultRho, 1e-3,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .3), vec2d(.7, .7)) } ),
				DefaultStopAfter, 50, DefaultFrameCapacity), "square obs., low visc., 256x256"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultRho, 1,
				ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "saddle flow, high visc."),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 128, 128,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultRho, 1,
				ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "saddle flow, high visc., 128x128"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, 256, 256,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultRho, 1,
				ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "saddle flow, high visc., 256x256"),

	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 1),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				BoundaryCond(BoundaryCondType::Neumann, vec2d(0, 0), BoundaryCondType::Dirichlet, 0),
				DefaultRho, 1e-3,
				ObstacleShapeStack(),
				DefaultStopAfter, 50, DefaultFrameCapacity), "saddle flow, low visc."),
};

}