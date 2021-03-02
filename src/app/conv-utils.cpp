/**
 * conv-utils.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/12/27
 */
#include "conv-utils.hpp"

namespace brandy0
{

bool ConvUtils::isPositiveReal(const std::string& s)
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

bool ConvUtils::isReal(const std::string& s)
{
    if (s.length() == 0)
        return false;
    else if (s[0] == '-')
        return isPositiveReal(s.substr(1));
    else
        return isPositiveReal(s);
}

bool ConvUtils::isNonnegativeInt(const std::string& s)
{
	for (const char c : s)
	{
		if (c < '0' || c > '9')
			return false;
	}
	return true;
}

bool ConvUtils::isNonzero(const std::string& s)
{
	for (const char c : s)
	{
		if (c != '0')
			return true;
	}
	return false;
}

bool ConvUtils::boundedStoi(const std::string& s, uint32_t& writeto, const uint32_t maxVal)
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
	const std::string entered = aentry.getText();
	if (entered == "" || !isPositiveReal(entered))
		aentry.indicateInvalid("enter a positive integer");
	else if (!boundedStoi(entered, writeto, maxVal))
		aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
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
	const std::string entered = aentry.getText();
	if (entered == "" || !isPositiveReal(entered))
	{
		aentry.indicateInvalid("enter a positive real number");
	}
	else
	{
		const double val = std::stod(entered, nullptr);
		if (val == 0)
		{
			aentry.indicateInvalid("enter a positive real number");
		}
		else if (val < minVal)
		{
			aentry.indicateInvalid("allowed min. is " + std::to_string(minVal));
		}
		else if (val > maxVal)
		{
			aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
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
    const std::string entered = aentry.getText();
    if (entered == "" || !isReal(entered))
    {
        aentry.indicateInvalid("enter a real number");
    }
	else
	{
		const double val = std::stod(entered, nullptr);
		if (val < -maxVal)
		{
			aentry.indicateInvalid("allowed min. is " + std::to_string(-maxVal));
		}
		else if (val > maxVal)
		{
			aentry.indicateInvalid("allowed max. is " + std::to_string(maxVal));
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

}