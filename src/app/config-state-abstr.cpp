/**
 * config-state-abstr.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/23
 */
#include "config-state-abstr.hpp"

namespace brandy0
{

ConfigStateAbstr::ConfigStateAbstr(ApplicationAbstr *const app)
	: app(app)
{
}

}