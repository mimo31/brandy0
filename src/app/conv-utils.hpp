/**
 * conv-utils.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#ifndef CONV_UTILS_HPP
#define CONV_UTILS_HPP

#include <cstdint>
#include <string>

#include "glob.hpp"

#include "annotated-entry.hpp"
#include "str.hpp"
#include "tests.hpp"

namespace brandy0
{

class ConvUtils
{
friend Tests;
private:
	/**
	 * Determines if a string denotes a non-negative real number. 
	 * 
	 * A string denoting a non-negative real number
	 * * may not contain whitespace and may not be empty,
	 * * may contain at most one decimal dot (possibly as the first character),
	 * * may contain at most one character 'e' or 'E', which must be followed by an integer.
	 * 
	 * @param s string to analyze
	 * @return true iff the specified string denotes a non-negative real number
	 */
    static bool isNonnegativeReal(const str& s);
	/**
	 * Determines if a string denotes a real number.
	 * 
	 * A string denotes a real number iff it denotes a non-negative real number or 
	 * it starts with '-' and the rest denotes a non-negative real number.
	 * 
	 * @param s string to analyze
	 * @return true iff the specified string denotes a real number
	 */
    static bool isReal(const str& s);
    static bool isNonnegativeInt(const str& s);
	/**
	 * @param s string to analyze
	 * @return true iff the specified string contains at least one character different from '0'
	 */
    static bool isNonzero(const str& s);
    static bool boundedStoi(const str& s, uint32_t& writeto, uint32_t maxVal);

public:
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, uint32_t defaultVal, uint32_t maxVal);
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, uint32_t defaultVal, uint32_t minVal, uint32_t maxVal);
    static void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, double defaultVal, double minVal, double maxVal);
    static void updateRealIndicator(AnnotatedEntry& aentry, double& writeto, double defaultVal, double maxVal);

	static str defaultToString(double d);
	static str timeToString(double d);
	static str timeToString(double d, double order);
	static str percentageToString(double d);
	static str speedupToString(double d);
	static str intToZeropadString(uint32_t i, uint32_t width);
	static str intToZeropadStringByOrder(uint32_t i, uint32_t order);
};

}

#endif // CONV_UTILS_HPP