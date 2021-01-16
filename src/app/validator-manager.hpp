/**
 * validator-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#ifndef VALIDATOR_MANAGER_HPP
#define VALIDATOR_MANAGER_HPP

#include <vector>

#include "func-utils.hpp"

namespace brandy0
{

class ValidatorManager
{
private:
	std::vector<BoolFunc> validators;

public:
	void plug(const BoolFunc& validator);
	bool isAllValid() const;
};

}

#endif // VALIDATOR_MANAGER_HPP