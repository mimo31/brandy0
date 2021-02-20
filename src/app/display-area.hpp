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

#include "display-modes.hpp"
#include "sim-frame.hpp"
#include "simulation-state-abstr.hpp"
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

struct UniformLoc
{
	std::string name;
	GLuint *pos;

	UniformLoc(const std::string& name, GLuint *const pos) : name(name), pos(pos)
	{
	}
};

typedef std::vector<UniformLoc> UniformLocVec;

class DisplayArea : public Gtk::GLArea
{
private:
	SimulationStateAbstr *parent;

	double w, h;
	uint32_t wp, hp;
	double dx, dy;

	Glib::RefPtr<Gdk::GLContext> glContext;

	Grid<bool> solid;

	GLuint glWhiteProgram = 0;
	GLuint glWhiteMat = 0;
	GLuint glPaintProgram = 0;
	GLuint glPaintMat = 0;
	GLuint glWhiteVao = 0;
	GLuint glWhiteVbo = 0;
	GLuint glPaintVao = 0;
	GLuint glPaintVbo = 0;

	void realize();
	void unrealize();
	bool render(const Glib::RefPtr<Gdk::GLContext>& context);

	void initBuffers();
	//GLuint loadProgram(const std::string& vshaderName, const std::string& fshaderName);
	void initShaders();

	Point to_poi(const double x, const double y);
	Point to_poi(const vec2d& v);
	vec2d to_coor(const uint32_t x, const uint32_t y);
	vec2d to_coor(const Point p);
	void computeMat(float *mat);

	void addStreamLine(std::vector<LineSegment>&, const vec2d&);
	void addArrow(std::vector<LineSegment>& segs, const vec2d& pos);
	void drawContent();

public:
	DisplayArea(SimulationStateAbstr *const parent);
	void redraw();
};

}

#endif // DISPLAY_AREA_HPP
