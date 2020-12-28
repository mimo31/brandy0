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

#include "glob.h"

#include "annotated-entry.hpp"

namespace brandy0
{

class ConvUtils
{
private:
    static bool isPositiveReal(const std::string& s);
    static bool isReal(const std::string& s);
    static bool isNonnegativeInt(const std::string& s);
    static bool isNonzero(const std::string& s);
    static bool boundedStoi(const std::string& s, uint32_t& writeto, const uint32_t maxVal);

public:
    static void updatePosIntIndicator(AnnotatedEntry& aentry, uint32_t& writeto, const uint32_t defaultVal, const uint32_t maxVal);
    static void updatePosRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double minVal, const double maxVal);
    static void updateRealIndicator(AnnotatedEntry& aentry, double& writeto, const double defaultVal, const double maxVal);

};

}

#endif // CONV_UTILS_HPP