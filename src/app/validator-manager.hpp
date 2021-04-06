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

/**
 * Container for arbitrarily many bool(void) callbacks.
 * Callbacks (a.k.a. validators) can be dynamically added and it can be queries whether they all evaluate to true.
 * Meant primarily for user input validation -- each entry can have its validator and the form can be submitted only if all validators evaluate to true.
 */
class ValidatorManager
{
private:
	/// Vector of all registered validators
	vec<BoolFunc> validators;

public:
	/**
	 * Registers a new validator
	 * @param validator validator to register
	 */
	void plug(const BoolFunc& validator);
	/**
	 * @return true iff all currently registered validators evaluate to true
	 */
	bool isAllValid() const;
};

}

#endif // VALIDATOR_MANAGER_HPP