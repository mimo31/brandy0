/**
 * func-utils.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#ifndef FUNC_UTILS_HPP
#define FUNC_UTILS_HPP

#include <functional>

namespace brandy0
{

typedef std::function<void()> VoidFunc;
typedef std::function<bool()> BoolFunc;

}

#endif // FUNC_UTILS_HPP