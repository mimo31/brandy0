/**
 * func.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef FUNC_HPP
#define FUNC_HPP

#include <functional>

namespace brandy0
{

typedef std::function<void()> VoidFunc;
typedef std::function<bool()> BoolFunc;

}

#endif // FUNC_HPP