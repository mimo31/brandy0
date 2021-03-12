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

void Tests::run()
{
	testConv();
}

}