/**
 * tests.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/01
 */
#include "tests.hpp"

#include <cassert>

#include "conv-utils.hpp"
#include "obstacle-shape.hpp"

namespace brandy0
{

void Tests::testConv()
{
	assert(ConvUtils::isNonnegativeReal("12345"));
	assert(ConvUtils::isNonnegativeReal("40e2"));
	assert(ConvUtils::isNonnegativeReal(".2e2"));
	assert(ConvUtils::isNonnegativeReal("1e-200"));
	assert(ConvUtils::isNonnegativeReal("0"));
	assert(ConvUtils::isNonnegativeReal("0.0"));
	assert(ConvUtils::isNonnegativeReal("0e10"));

	assert(!ConvUtils::isNonnegativeReal(""));
	assert(!ConvUtils::isNonnegativeReal("."));
	assert(!ConvUtils::isNonnegativeReal("1."));
	assert(!ConvUtils::isNonnegativeReal("e"));
	assert(!ConvUtils::isNonnegativeReal("e2"));
	assert(!ConvUtils::isNonnegativeReal("1e.3"));
	assert(!ConvUtils::isNonnegativeReal("-"));
	assert(!ConvUtils::isNonnegativeReal("-e2"));
	assert(!ConvUtils::isNonnegativeReal("-.2"));

	assert(ConvUtils::isReal("0"));
	assert(ConvUtils::isReal("0.0"));
	assert(ConvUtils::isReal("0e10"));
	assert(ConvUtils::isReal("-0"));
	assert(ConvUtils::isReal("-0.0"));
	assert(ConvUtils::isReal("-0e10"));
	assert(ConvUtils::isReal("-.2"));
	
	assert(!ConvUtils::isReal("-e2"));
}

void Tests::testObstacleShapes()
{
	const ObstaclePolygon tria(false, vec<vec2d>{ vec2d(0, 0), vec2d(1, 0), vec2d(0, 1) });
	assert(tria.inside(vec2d(0, 0)));
	assert(tria.inside(vec2d(.5, .4)));
	assert(tria.inside(vec2d(.1, .755)));
	assert(tria.inside(vec2d(0, 1)));

	assert(!tria.inside(vec2d(1.1, .2)));
	assert(!tria.inside(vec2d(-.01, .3)));
	assert(!tria.inside(vec2d(.4, .64)));
	assert(!tria.inside(vec2d(0, 1.5)));

	const ObstaclePolygon singleton(false, vec<vec2d>{ vec2d(0, 0), vec2d(0, 0), vec2d(0, 0) });
	assert(singleton.inside(vec2d(0, 0)));

	assert(!singleton.inside(vec2d(1, 0)));
	assert(!singleton.inside(vec2d(-1, -1)));
	assert(!singleton.inside(vec2d(-3.555e2, 7)));
}

void Tests::run()
{
	testConv();
	testObstacleShapes();
}

}