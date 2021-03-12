/**
 * tests.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/01
 */
#include "tests.hpp"

#include <cassert>

#include "conv-utils.hpp"

namespace brandy0
{

void Tests::testConv()
{
	assert(ConvUtils::isPositiveReal("12345"));
	assert(ConvUtils::isPositiveReal("40e2"));
	assert(ConvUtils::isPositiveReal(".2e2"));
	assert(ConvUtils::isPositiveReal("1e-200"));

	assert(!ConvUtils::isPositiveReal(""));
	assert(!ConvUtils::isPositiveReal("."));
	assert(!ConvUtils::isPositiveReal("1."));
	assert(!ConvUtils::isPositiveReal("e"));
	assert(!ConvUtils::isPositiveReal("e2"));
	assert(!ConvUtils::isPositiveReal("1e.3"));
	assert(!ConvUtils::isPositiveReal("-"));
	assert(!ConvUtils::isPositiveReal("-e2"));
	assert(!ConvUtils::isPositiveReal("-.2"));
	assert(!ConvUtils::isPositiveReal("0"));
	assert(!ConvUtils::isPositiveReal("0.0"));
}

void Tests::run()
{
	testConv();
}

}