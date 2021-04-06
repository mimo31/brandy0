/**
 * brandy-window.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/01/14
 */
#include "brandy-window.hpp"

namespace brandy0
{

BrandyWindow::BrandyWindow()
{
	set_title("brandy0");
	set_border_width(12);
}

BrandyWindow::BrandyWindow(const uint32_t defaultWidth, const uint32_t defaultHeight)
	: BrandyWindow()
{
	set_default_size(defaultWidth, defaultHeight);
}

}