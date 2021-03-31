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

class State
{
public:
	virtual void deactivate() = 0;
	virtual ~State() { };
};

}

#endif // STATE_HPP
