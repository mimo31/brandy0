/**
 * start-state-abstr.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/03/02
 */
#ifndef START_STATE_ABSTR_HPP
#define START_STATE_ABSTR_HPP

namespace brandy0
{

class StartStateAbstr
{
public:
	virtual void goToNewSimulation() = 0;
	virtual void showAbout() = 0;
};

}

#endif // START_STATE_ABSTR_HPP