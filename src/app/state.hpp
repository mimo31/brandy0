/**
 * state.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/2
 */
#ifndef STATE_HPP
#define STATE_HPP

namespace brandy0
{

/**
 * Abstract interface for a state of the application.
 * Provides a deactivate method so that when the current state is being changed, the previous current state can do what it needs to do (e.g. hide its windows)
 */
class State
{
public:
	/**
	 * Deactivates the state, however the state needs to get deactivated.
	 * Should be called whenever the state is starting to no longer be the current state.
	 */
	virtual void deactivate() = 0;
	virtual ~State() { };
};

}

#endif // STATE_HPP
