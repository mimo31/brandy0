/**
 * simulation-params-preset.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#include "simulation-params-preset.hpp"

namespace brandy0
{

const BoundaryCond SimulationParamsPreset::defaultBc(DefaultVelocityType, vec2d(DefaultU, DefaultU), DefaultPressureType, DefaultP);

const SimulationParams SimulationParamsPreset::defaultParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, defaultBc, defaultBc, defaultBc, defaultBc, DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity);

const vec<SimulationParamsPreset> SimulationParamsPreset::presets {
	/*SimulationParamsPreset(SimulationParamsPreset::defaultParams, "static (default)"),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(0, .1), PressureBoundaryCond::DIRICHLET), defaultBc, defaultBc, defaultBc,
				DefaultRho, DefaultMu, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, high visc."),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(0, .1), PressureBoundaryCond::DIRICHLET), defaultBc, defaultBc, defaultBc,
				DefaultRho, .03, ObstacleShapeStack(),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "cavity flow, low visc."),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DefaultRho, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .35), vec2d(.6, .65)) } ),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "big square obs."),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DefaultRho, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.4, .45), vec2d(.5, .55)) } ),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "small square obs."),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DefaultRho, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleCircle>(false, vec2d(.45, .5), .15) } ),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "big circle obs."),
	SimulationParamsPreset(SimulationParams(DefaultW, DefaultH, DefaultWp, DefaultHp,
			   	DefaultDt, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DefaultRho, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleCircle>(false, vec2d(.45, .5), .05) } ),
				DefaultStopAfter, DefaultStepsPerFrame, DefaultFrameCapacity), "small circle obs."),*/
};

}