/**
 * ptr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#ifndef PTR_HPP
#define PTR_HPP

#include <memory>

namespace brandy0
{

template <typename T>
using uptr = std::unique_ptr<T>;

using std::make_unique;

template <typename T>
using sptr = std::unique_ptr<T>;

}

#endif // PTR_HPP