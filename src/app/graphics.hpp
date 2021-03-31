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

/**
 * Manages the program's resources used for OpenGL drawing.
 */
class GraphicsManager
{
private:
	/// true iff the init method has already been called on this instance
	bool initialized = false;

	/**
	 * Creates the required OpenGL buffers and sets the attributes which should contain the numbers of the buffers
	 */
	void initBuffers();
	/**
	 * Load and compiles the required OpenGL shaders and programs and sets the attributes which should contain the numbers of the shaders and the programs
	 */
	void initShaders();
public:
	/// Number of the compiled shader program for drawing white objects
	GLuint glWhiteProgram;
	/// Number of the uniform for the transformation matrix inside the program for drawing white objects
	GLuint glWhiteMat;
	/// Number of the compiled shader program for drawing in 1D (rainbow) color
	GLuint glPaintProgram;
	/// Number of the uniform for the transformation matrix inside the program for drawing in 1D (rainbow) color
	GLuint glPaintMat;
	/// Number of the Vertex Array Object for the program for white objects
	GLuint glWhiteVao;
	/// Number of the Vertex Buffer Object for the program for white objects
	GLuint glWhiteVbo;
	/// Number of the Vertex Array Object for the program for rainbow objects
	GLuint glPaintVao;
	/// Number of the Vertex Buffer Object for the program for rainbow objects
	GLuint glPaintVbo;

	/**
	 * Context for OpenGL drawing.
	 * This attribute should be set externally by the DisplayArea when it realizes.
	 * It (this attribute) is independent on the state of the buffers, programs, and uniforms attributes
	 */
	Glib::RefPtr<Gdk::GLContext> ctx;

	/**
	 * Initializes all the number of something attributes by creating the OpenGL objects accoringly.
	 * Does nothing if this method has already been called on this instance.
	 */
	void init();
	/**
	 * Invalidates resource claims held by this instance. In particular, clears the pointer to an OpenGL context
	 */
	void destruct();
};

/**
 * Draws the visualizations (given computed data in the form of frames).
 * Can draw to the DisplayArea (by directly using the provided OpenGL context),
 * or to a byte array (by creating a newframe buffer)
 */
class FrameDrawer
{
private:
	/**
	 * A representation of a line segment in 2D as a pair of two 2D vectors.
	 */
	struct LineSegment
	{
		/// One endpoint of the line segment
		vec2d p0;
		/// The other endpoint of the line segment
		vec2d p1;

		/**
		 * Constructs a LineSegment object
		 * @param p0 one endpoint
		 * @param p1 the other endpoint
		 */
		LineSegment(const vec2d &p0, const vec2d &p1) : p0(p0), p1(p1)
		{
		}
	};

	/// Physical width of the simulated fluid container
	double w;
	/// Physical height of the simulated fluid container
	double h;
	/// Width of the simulation grid
	uint32_t wp;
	/// Height of the simulation grid
	uint32_t hp;
	/// Spacial grid step in the x direction
	double dx;
	/// Spacial grid step in the y direction
	double dy;
	/// Width (in pixels) of the target area that should be drawn into
	double vieww;
	/// Height (in pixels) of the target area that should be drawn into
	double viewh;

	/// Grid of whether any grid point is solid
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