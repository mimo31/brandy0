/**
 * display-area.hpp
 *
 * Author: Viktor Fukala
 * Created on 2020/12/25
 */
#ifndef DISPLAY_AREA_HPP
#define DISPLAY_AREA_HPP

#include <gtkmm/glarea.h>
#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "glob.hpp"

#include "sim-frame.hpp"
#include "simulator-params.hpp"
#include "vec2d.hpp"

namespace brandy0
{

struct LineSegment
{
	vec2d p0, p1;

	LineSegment(const vec2d& p0, const vec2d& p1) : p0(p0), p1(p1)
	{
	}
};

class DisplayArea : public Gtk::GLArea
{
private:
	Glib::RefPtr<Gdk::GLContext> glContext;

	const SimulatorParams *params;
	const SimFrame *curFrame;

	GLuint glProgram = 0;
	GLuint glVao = 0;
	GLuint glVbo = 0;
	GLuint glMat = 0;

	void realize();
	void unrealize();
	bool render(const Glib::RefPtr<Gdk::GLContext>& context);

	void initBuffers();
	void initShaders();

	Point to_poi(const vec2d& v);
	void computeMat(float *mat);

	void addStreamLine(std::vector<LineSegment>&, const vec2d&);
	void drawContent();


public:
	DisplayArea();
	void setParams(const SimulatorParams *const);
	void setCurFrame(const SimFrame *const);
};

}

#endif // DISPLAY_AREA_HPP
