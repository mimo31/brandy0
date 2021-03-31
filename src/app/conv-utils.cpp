/**
 * conv-utils.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#include "conv-utils.hpp"

#include <iomanip>

namespace brandy0
{

bool ConvUtils::isNonnegativeReal(const str& s)
{
	uint32_t ind = 0;
	while (ind < s.length() && s[ind] != 'e' && s[ind] != 'E')
		ind++;
	if (ind == 0 || ind == s.length() - 1)
		return false;
	uint32_t dotind = UINT32_MAX;
	for (uint32_t i = 0; i < ind; i++)
	{
		if (s[i] == '.')
		{
			dotind = i;
			break;
		}
	}
	if (dotind == ind - 1)
		return false;
	if (dotind == UINT32_MAX)
		dotind = ind;
	for (uint32_t i = 0; i < dotind; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	for (uint32_t i = dotind + 1; i < ind; i++)
		if (s[i] < '0' || s[i] > '9')
			return false;
	if (ind != s.length())
	{
		if (s[ind + 1] == '-' && ind + 1 == s.length() - 1)
			return false;
		for (uint32_t i = (s[ind + 1] == '-' ? ind + 1 : ind) + 1; i < s.length(); i++)
			if (s[i] < '0' || s[i] > '9')
				return false;
	}
	return true;
}

bool ConvUtils::isReal(const str& s)
{
	if (s.length() == 0)
		return false;
	if (s[0] == '-')
		return isNonnegativeReal(s.substr(1));
	return isNonnegativeReal(s);
}

bool ConvUtils::isNonnegativeInt(const str& s)
{
	return !s.empty() && std::all_of(s.begin(), s.end(), [](const char c){ return c >= '0' && c <= '9'; });
}

bool ConvUtils::isNonzero(const str& s)
{
	return std::any_of(s.begin(), s.end(), [](const char c){ return c != '0'; });
}

bool ConvUtils::boundedStoi(const str& s, uint32_t& writeto, const uint32_t maxVal)
{
	uint32_t val = 0;
	for (const char c : s)
	{
		val = val * 10 + c - '0';
		if (val > maxVal)
			return false;
	}
	writeto = val;
	return true;
}

void ConvUtils::updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal)
{
	updatePosIntIndicator(aentry, writeto, defaultVal, 1, maxVal);
}

void ConvUtils::updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t minVal, const uint32_t maxVal)
{
	const str entered = aentry.getText();
	if (entered.empty() || !isNonnegativeInt(entered) || !isNonzero(entered))
		aentry.indicateInvalid("enter a positive integer");
	else if (!boundedStoi(entered, writeto, maxVal))
		aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
	else if (writeto < minVal)
		aentry.indicateInvalid("allowed min. is " + std::to_string(minVal));
	else
	{
		if (writeto == defaultVal)
			aentry.indicateDefault();
		else
			aentry.indicateOk();
	}
}

void ConvUtils::updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal)
{
	const str entered = aentry.getText();
	if (entered.empty() || !isNonnegativeReal(entered))
	{
		aentry.indicateInvalid("enter a positive real number");
	}
	else
	{
		const double val = strtod(entered.c_str(), nullptr);
		if (val == 0)
		{
			aentry.indicateInvalid("enter a positive real number");
		}
		else if (val < minVal || val == -HUGE_VAL)
		{
			aentry.indicateInvalid("allowed min. is " + defaultToString(minVal));
		}
		else if (val > maxVal || val == HUGE_VAL)
		{
			aentry.indicateInvalid("allowed max. is " + defaultToString(maxVal));
		}
		else
		{
			writeto = val;
			if (val == defaultVal)
				aentry.indicateDefault();
			else
				aentry.indicateOk();
		}
	}
}

void ConvUtils::updateRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double maxVal)
{
	const str entered = aentry.getText();
	if (entered.empty() || !isReal(entered))
	{
		aentry.indicateInvalid("enter a real number");
	}
	else
	{
		const double val = strtod(entered.c_str(), nullptr);
		if (val < -maxVal || val == -HUGE_VAL)
		{
			aentry.indicateInvalid("allowed min. is " + defaultToString(-maxVal));
		}
		else if (val > maxVal || val == HUGE_VAL)
		{
			aentry.indicateInvalid("allowed max. is " + defaultToString(maxVal));
		}
		else
		{
			writeto = val;
			if (val == defaultVal)
				aentry.indicateDefault();
			else
				aentry.indicateOk();
		}
	}
}

str ConvUtils::defaultToString(const double d)
{
	std::ostringstream oss;
	oss << d;
	return oss.str();
}

str ConvUtils::timeToString(const double d)
{
	return timeToString(d, d);
}

/**
 * Converts a real number to a string in scientific notation
 * @param d real number to convert
 * @return converted string
 */
str toScientific(const double d)
{
	std::ostringstream oss;
	oss << std::scientific;
	oss.precision(3);
	oss << d;
	return oss.str();
}

/**
 * Converts a real number to a string in fixed point notation (not scientific) with a given number of decimal places
 * @param d real number to convert
 * @param prec number of decimal places (after the decimal point)
 * @return converted string
 */
str toFixed(const double d, const uint32_t prec)
{
	std::ostringstream oss;
	oss << std::fixed;
	oss.precision(prec);
	oss << d;
	return oss.str();
}

str ConvUtils::timeToString(const double d, const double order)
{
	if (order == 0)
		return defaultToString(d);
	if (order < 1e-4)
		return toScientific(d);
	if (order >= 1e6)
		return toScientific(d);
	if (order >= 1e5)
		return toFixed(d, 0);
	double pow = 1e-3;
	int32_t prec = 7;
	while (order >= pow)
	{
		pow *= 10;
		prec--;
	}
	return toFixed(d, std::max(0, prec));
}

str ConvUtils::percentageToString(const double d)
{
	return toFixed(d, 3);
}

str ConvUtils::speedupToString(const double d)
{
	if (d >= 100)
		return toFixed(d, 0);
	if (d >= .1)
		return toFixed(d, 2);
	return toFixed(d, 5);
}

str ConvUtils::intToZeropadString(const uint32_t i, const uint32_t width)
{
	std::ostringstream oss;
	oss << std::setw(width) << std::setfill('0') << i;
	return oss.str();
}

str ConvUtils::intToZeropadStringByOrder(const uint32_t i, const uint32_t order)
{
	uint32_t width = 1;
	uint32_t bound = 10;
	while (bound <= order)
	{
		width++;
		bound *= 10;
	}
	return intToZeropadString(i, width);
}

}