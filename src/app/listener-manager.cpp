/**
 * listener-manager.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/1/16
 */
#include "listener-manager.hpp"

namespace brandy0
{

void ListenerManager::plug(const VoidFunc& listener)
{
	listeners.push_back(listener);
}

void ListenerManager::invoke() const
{
	for (const VoidFunc& l : listeners)
		l();
}

}