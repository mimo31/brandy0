/**
 * simulation-params-preset.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/07
 */
#include "simulation-params-preset.hpp"

namespace brandy0
{

const BoundaryCond SimulationParamsPreset::defaultBc(vec2d(DEFAULT_U, DEFAULT_U), DEFAULT_PRESSURE_BC);

const SimulationParams SimulationParamsPreset::defaultParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, defaultBc, defaultBc, defaultBc, defaultBc, DEFAULT_RHO, DEFAULT_MU, ObstacleShapeStack(),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY);

const vec<SimulationParamsPreset> SimulationParamsPreset::presets {
	SimulationParamsPreset(SimulationParamsPreset::defaultParams, "static (default)"),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(0, .1), PressureBoundaryCond::DIRICHLET), defaultBc, defaultBc, defaultBc,
				DEFAULT_RHO, DEFAULT_MU, ObstacleShapeStack(),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "cavity flow, high visc."),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(0, .1), PressureBoundaryCond::DIRICHLET), defaultBc, defaultBc, defaultBc,
				DEFAULT_RHO, .03, ObstacleShapeStack(),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "cavity flow, low visc."),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DEFAULT_RHO, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.3, .35), vec2d(.6, .65)) } ),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "big square obs."),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DEFAULT_RHO, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleRectangle>(false, vec2d(.4, .45), vec2d(.5, .55)) } ),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "small square obs."),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DEFAULT_RHO, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleCircle>(false, vec2d(.45, .5), .15) } ),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "big circle obs."),
	SimulationParamsPreset(SimulationParams(DEFAULT_W, DEFAULT_H, DEFAULT_WP, DEFAULT_HP,
			   	DEFAULT_DT, BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET), BoundaryCond(vec2d(.4, 0), PressureBoundaryCond::DIRICHLET),
				defaultBc, defaultBc, DEFAULT_RHO, .1,
				ObstacleShapeStack(vec<sptr<ObstacleShape>> { make_shared<ObstacleCircle>(false, vec2d(.45, .5), .05) } ),
				DEFAULT_STOP_AFTER, DEFAULT_STEPS_PER_FRAME, DEFAULT_FRAME_CAPACITY), "small circle obs."),
};

}