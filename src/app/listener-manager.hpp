/**
 * listener-manager.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/16
 */
#ifndef LISTENER_MANAGER_HPP
#define LISTENER_MANAGER_HPP

#include "func.hpp"
#include "vec.hpp"

namespace brandy0
{

/**
 * A container for arbitrarily many void(void) callbacks.
 * Meant primarily for events -- call plug to register a listener, call invoke to fire the event.
 */
class ListenerManager
{
private:
	/// All the registered listners
	vec<VoidFunc> listeners;

public:
	/**
	 * Registers a new listener
	 * @param listener listener to register
	 */
	void plug(const VoidFunc &listener);
	/**
	 * Calls all the previously registered listeners
	 */
	void invoke() const;
};

}

#endif // LISTENER_MANAGER_HPP