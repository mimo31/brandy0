/**
 * validator-manager.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#include "validator-manager.hpp"

namespace brandy0
{

void ValidatorManager::plug(const BoolFunc& validator)
{
	validators.push_back(validator);
}

bool ValidatorManager::isAllValid() const
{
	return std::all_of(validators.begin(), validators.end(), [](const BoolFunc& v){ return v(); });
}

}