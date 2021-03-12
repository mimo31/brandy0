/**
 * conv-utils.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#include "conv-utils.hpp"

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
    else if (s[0] == '-')
        return isNonnegativeReal(s.substr(1));
    else
        return isNonnegativeReal(s);
}

bool ConvUtils::isNonnegativeInt(const str& s)
{
	for (const char c : s)
	{
		if (c < '0' || c > '9')
			return false;
	}
	return true;
}

bool ConvUtils::isNonzero(const str& s)
{
	for (const char c : s)
	{
		if (c != '0')
			return true;
	}
	return false;
}

bool ConvUtils::boundedStoi(const str& s, uint32_t& writeto, const uint32_t maxVal)
{
	uint32_t val = 0;
	for (uint32_t i = 0; i < s.length(); i++)
	{
		val = val * 10 + s[i] - '0';
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
	if (entered == "" || !isNonnegativeInt(entered) || !isNonzero(entered))
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
	if (entered == "" || !isNonnegativeReal(entered))
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
    if (entered == "" || !isReal(entered))
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

}