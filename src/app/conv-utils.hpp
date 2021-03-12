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
    static bool boundedStoi(const str& s, uint32_t& writeto, const uint32_t maxVal);

public:
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal);
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t minVal, const uint32_t maxVal);
    static void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal);
    static void updateRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double maxVal);

	static str defaultToString(const double d);
};

}

#endif // CONV_UTILS_HPP