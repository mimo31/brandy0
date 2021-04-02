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

	/// Grid indicating whether a grid point is solid
	Grid<bool> solid;

	/// The selected foreground visual mode to draw
	uint32_t frontDisplayMode;
	/// The selected background visual mode to draw
	uint32_t backDisplayMode;

	/// True iff OpenGL structures (framebuffer and texture; used for drawing to byte arrays) have been created for this drawer
	bool has_gl_structs = false;
	/// The used OpenGL framebuffer (for drawing to byte arrays)
	GLuint gl_framebuf;
	/// The used OpenGL texture (for drawing to byte arrays)
	GLuint gl_texture;

	/**
	 * Creates the OpenGL structures required for drawing to byte arrays
	 * Does nothing if has_gl_structs is already true
	 * @see has_gl_structs, @see gl_framebuf, @see gl_texture
	 */ 
	void generate_gl_structs();

	/**
	 * Converts a physical point to a grid point
	 * @param x x physical coordinate of the point
	 * @param y y physical coordinate of the point
	 * @return grid point closest to the specified point
	 */
	Point to_poi(double x, double y) const;
	/**
	 * Converts a physical point to a grid point.
	 * Equivalent to to_poi(v.x, v.y).
	 */
	Point to_poi(const vec2d& v) const;
	/**
	 * Converts a grid point to physical coordinates
	 * @param x x coordinate of the grid point
	 * @param y y coordinate of the grid point
	 * @return physical coordinates of the grid point
	 */
	vec2d to_coor(uint32_t x, uint32_t y) const;
	/**
	 * Converts a grid point to physical coordinates.
	 * Equivalent to to_coor(p.x, p.y).
	 */
	vec2d to_coor(Point p) const;
	/**
	 * Computes the view matrix for the shader programs based on w, h, and the vieww / viewh ratio
	 * @param mat location to write the matrix entries to (writes to mat[0] -- mat[15])
	 */
	void computeMat(float *mat) const;
	
	/**
	 * Draws a streamline from a given point based on the velocity field of a given frame by writing to a vector of line segments
	 * @param frame frame with the velocity field to use
	 * @param segs vector of line segments to add segments to
	 * @param ini starting point of the streamline
	 */
	void addStreamline(const SimFrame &frame, vec<LineSegment> &segs, const vec2d &ini);
	/**
	 * Draws an arrow representing the velocity at a given point in a given frame by writing to a vector of line segments
	 * @param frame frame with the velocity field to use
	 * @param segs vector of line segments to add segment to
	 * @param pos physical point to represent the velocity field at
	 * @param norm_len unit length of the velocity vector (velocity vector of this length will always be drawn with a set, constant length). Increasing this parameter proportionally decreases the length of the drawn vectors and vice versa
	 */
	void addArrow(const SimFrame &frame, vec<LineSegment> &segs, const vec2d &pos, double norm_len);

	/**
	 * Draws the entire frame based on the parameters set in this object's attributes
	 * @param frame frame to draw
	 * @param manager GraphicsManager to get the numbers of the shader programs and their uniforms
	 */
	void drawAll(const SimFrame &frame, GraphicsManager *manager);

public:
	/**
	 * Constructs a FrameDrawer object
	 * @param p parameters of the simulation, computed frames of which will later be drawn
	 */
	FrameDrawer(const SimulationParams &p);

	/**
	 * Sets the foreground visual mode for future frame draws
	 * @param fdm index of the foreground visual mode
	 */
	void setFrontDisplayMode(uint32_t fdm);
	/**
	 * Sets the background visual mode for future frame draws
	 * @param bdm index of the background visual mode
	 */
	void setBackDisplayMode(uint32_t bdm);

	/**
	 * Draws a frame based on the simulation parameters passed in this object's constructor and previously set visual modes. Draws to the current target in manager->ctx (most likely the DisplayArea).
	 * @param frame frame to draw
	 * @param view_width width of the area to draw to
	 * @param view_height height of the area to draw to
	 * @param manager GraphicsManager with the necessary shader program and uniform numbers
	 */
	void drawFrame(const SimFrame& frame, double view_width, double view_height, GraphicsManager *manager);
	/**
	 * Draws a frame based on the simulation parameters passed in this object's constructor and previsously set visual modes. Draws to a byte array in RGB format.
	 * @param frame frame to draw
	 * @param width width of the pixel grid to draw to
	 * @param height height of the pixel grid to draw to
	 * @param data array to draw (write bytes) to; writes to data[0] to data[3 * height * linesize - 1]
	 * @param linesize number of bytes per horizontal line; should be >= 3 * width
	 * @param manager GraphicsManager with the necessary shader program and uniform numbers and the OpenGL context to make current
	 */
	void drawFrame(const SimFrame& frame, uint32_t width, uint32_t height, uint8_t *data, uint32_t linesize, GraphicsManager *manager);
};

}

#endif // GRAPHICS_HPP