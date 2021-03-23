/**
 * validator-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef VALIDATOR_MANAGER_HPP
#define VALIDATOR_MANAGER_HPP

#include "func.hpp"
#include "vec.hpp"

namespace brandy0
{

class ValidatorManager
{
private:
	vec<BoolFunc> validators;

public:
	void plug(const BoolFunc& validator);
	bool isAllValid() const;
};

}

#endif // VALIDATOR_MANAGER_HPP