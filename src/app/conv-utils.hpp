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

#include "annotated-entry.hpp"
#include "str.hpp"
#include "tests.hpp"

namespace brandy0
{

/**
 * Static class with utility functions for converting between string and number types
 * and generally validating user input.
 */
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
	/**
	 * Determines if a string denotes a non-negative integer.
	 *
	 * That is iff the string is non-empty and consists of no other characters than the digits (0 to 9).
	 * 
	 * @param s string to analyze
	 * @return true iff the specified string denotes a non-negative integer
	 */
    static bool isNonnegativeInt(const str& s);
	/**
	 * @param s string to analyze
	 * @return true iff the specified string contains at least one character different from '0'
	 */
    static bool isNonzero(const str& s);
	/**
	 * Converts a string to an integer while checking for overflows.
	 * @param s string to convert
	 * @param writeto reference to which the result (the converted integer) should be written
	 * @param maxVal the maximum value that the converted integer can have
	 * If maxVal >= C / 10, where C is the maximum value of a uint32_t variable, and the actual value of the integer is greater than the maximum value of uint32_t,
	 * the behavior is undefined.
	 * @return true iff the conversion succeeded
	 * writeto is written to iff the conversion succeeded (this function returns true)
	 */
    static bool boundedStoi(const str& s, uint32_t& writeto, uint32_t maxVal);

public:
	/**
	 * Parses a positive integer from an AnnotatedEntry and updates its indicator accordinlgy.
	 * Equivalent to updatePosIntIndicator(aentry, writeto, defaultVal, 0, maxVal).
	 */
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, uint32_t defaultVal, uint32_t maxVal);
	/**
	 * Parses a positive integer from an AnnotatedEntry and updates its indicator accordingly.
	 * @param aentry the AnnotatedEntry to be read and updated
	 * @param writeto reference to write the parsed integer to (iff the input is valid)
	 * @param defaultVal the default value of the quantity that is being entered (to possibly set the aentry's indicator state to default)
	 * @param minVal the minimum value (to possibly invalidate the input)
	 * @param maxVal the maximum value (to possibly invalidate the input)
	 */
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, uint32_t defaultVal, uint32_t minVal, uint32_t maxVal);
	/**
	 * Parses a positive real number from an AnnotatedEntry and updates its indicator accordingly.
	 * @param aentry the AnnotatedEntry to be read and updated
	 * @param writeto reference to write the parsed real number to (iff the input is valid)
	 * @param defaultVal the default value of the quantity that is being entered (to possibly set the aentry's indicator state to default)
	 * @param minVal the minimum value (to possibly invalidate the input)
	 * @param maxVal the maximum value (to possibly invalidate the input)
	 */
    static void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, double defaultVal, double minVal, double maxVal);
	/**
	 * Parses a real number from an AnnotatedEntry and updates its indicator accordingly.
	 * @param aentry the AnnotatedEntry to be read and updated
	 * @param writeto reference to write the parsed real number to (iff the input is valid)
	 * @param defaultVal the default value of the quantity that is being entered (to possibly set the aentry's indicator state to default)
	 * @param maxVal the maximum absolute value (to possibly invalidate the input)
	 */
    static void updateRealIndicator(AnnotatedEntry& aentry, double& writeto, double defaultVal, double maxVal);

	/**
	 * Converts a real number to a string in the default format (i.e. the format given by sending a double to a stream)
	 * @param d real number to convert
	 * @return converted string
	 */
	static str defaultToString(double d);
	/**
	 * Converts a real number representing a quantity of simulation time to a string.
	 * Equivalent to timeToString(d, d).
	 */
	static str timeToString(double d);
	/**
	 * Converts a real number representing a quantity of simulation time to a string
	 * in the special format (e.g. the number of decimal places) used for simulation time
	 * @param d real number specifying some simulation time to be converted
	 * @param order real number specifying some simulation time that will be used to decide the format of the converted string
	 * @return converted string
	 */
	static str timeToString(double d, double order);
	/**
	 * Converts a real number representing some percentage to a string
	 * in the special format used for percentages
	 * @param d percentage to convert (in the interval [0, 100])
	 * @return converted string
	 */
	static str percentageToString(double d);
	/**
	 * Converts a real number representing the speedup of simulation playback to a string
	 * in the special format used for playback speedup
	 * @param d speedup value to convert (1 is no speedup, < 1 is slower, > 1 faster)
	 * @return converted string
	 */
	static str speedupToString(double d);
	/**
	 * Converts an integer to string of a given length such that the integer's digits are preceded by zeros ('0')
	 * if the number of the integer's own digits is smaller than the width of the string.
	 * @param i integer to convert
	 * @param width width of the resulting string
	 * @return string with the converted integer and potentially padded with zeros
	 */
	static str intToZeropadString(uint32_t i, uint32_t width);
	/**
	 * Converts an integer to a string of a fixed length given by the number of digits of another integer such that
	 * the digits of the converted integer are preceded by zeros ('0') if their number is smaller
	 * than the number of digits of the other integer
	 * @param i integer to convert; should be <= order
	 * @param order the integer specifying the length of the resulting string
	 * @return string with the converted integer and potentially padded with zeros
	 */
	static str intToZeropadStringByOrder(uint32_t i, uint32_t order);
};

}

#endif // CONV_UTILS_HPP