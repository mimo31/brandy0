/**
 * hideable.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/18
 */
#ifndef HIDEABLE_HPP
#define HIDEABLE_HPP

#include <cairomm/context.h>
#include <cairomm/refptr.h>

namespace brandy0
{

template<typename T>
class Hideable : public T
{
private:
	bool hidden = false;
	bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override
	{
		if (!hidden)
			return T::on_draw(cr);
		else
			return true;
	}

public:
	template<typename... CtorArgs>
	Hideable(CtorArgs... args) : T(args...)
	{
	} 

	void pseudoHide()
	{
		hidden = true;
		T::set_sensitive(false);
	}

	void pseudoShow()
	{
		hidden = false;
		T::set_sensitive(true);
	}
};

}

#endif // HIDEABLE_HPP