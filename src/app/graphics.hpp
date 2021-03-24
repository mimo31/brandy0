/**
 * graphics.hpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/21
 */
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cstdint>

#include <epoxy/gl.h>
#include <gtkmm/glarea.h>
#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "sim-frame.hpp"
#include "simulation-params.hpp"

namespace brandy0
{

class GraphicsManager
{
private:
	bool initialized = false;
public:
	GLuint glWhiteProgram;
	GLuint glWhiteMat;
	GLuint glPaintProgram;
	GLuint glPaintMat;
	GLuint glWhiteVao;
	GLuint glWhiteVbo;
	GLuint glPaintVao;
	GLuint glPaintVbo;

	Glib::RefPtr<Gdk::GLContext> ctx;

	void initBuffers();
	void initShaders();

	void init();
	void destruct();
};

class FrameDrawer
{
private:
	struct LineSegment
	{
		vec2d p0, p1;

		LineSegment(const vec2d &p0, const vec2d &p1) : p0(p0), p1(p1)
		{
		}
	};

	double w, h;
	uint32_t wp, hp;
	double dx, dy;
	double vieww, viewh;

	Grid<bool> solid;

	uint32_t frontDisplayMode, backDisplayMode;

	bool has_gl_structs = false;
	GLuint gl_framebuf, gl_texture;

	void generate_gl_structs();

	Point to_poi(double x, double y) const;
	Point to_poi(const vec2d& v) const;
	vec2d to_coor(uint32_t x, uint32_t y) const;
	vec2d to_coor(Point p) const;
	void computeMat(float *mat) const;
	
	void addStreamLine(const SimFrame& frame, vec<LineSegment>&, const vec2d&);
	void addArrow(const SimFrame& frame, vec<LineSegment>& segs, const vec2d& pos, double norm_len);

	void drawAll(const SimFrame& frame, GraphicsManager *manager);

public:
	FrameDrawer(const SimulationParams& p);

	void setFrontDisplayMode(uint32_t fdm);
	void setBackDisplayMode(uint32_t bdm);

	void drawFrame(const SimFrame& frame, double view_width, double view_height, GraphicsManager *manager);
	void drawFrame(const SimFrame& frame, uint32_t width, uint32_t height, uint8_t *data, uint32_t linesize, GraphicsManager *manager);
};

}

#endif // GRAPHICS_HPP