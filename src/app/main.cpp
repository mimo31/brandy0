#include <cstdlib>
#include <iostream>
#include <vector>

#include <gtkmm.h>
#include <giomm/resource.h>
#include <epoxy/gl.h>

#include "point.hpp"
#include "simulator.hpp"
#include "vec2d.hpp"

namespace brandy0
{

using std::cout;
using std::endl;
using std::cerr;

uint32_t glarea_w;
uint32_t glarea_h;
uint32_t wp, hp;
double w, h;

struct LineSegment
{
	vec2d p0, p1;

	LineSegment(const vec2d& p0, const vec2d& p1) : p0(p0), p1(p1)
	{
	}
};

class GLAreaWindow : public ::Gtk::Window
{
public:
	GLAreaWindow();
	~GLAreaWindow() override;
	void set_u(const Grid<vec2d>&);
	void set_solid(const Grid<bool>&);

protected:
	Glib::RefPtr<Gdk::GLContext> gl_context;
	Gtk::GLArea gl_area;

	GLuint gl_program{0};
	GLuint gl_vao{0};
	GLuint gl_vbo{0};
	GLuint gl_mat{0};
	
	void realize();
	void unrealize();
	bool render(const Glib::RefPtr<Gdk::GLContext>& context);

	void init_buffers();
	void init_shaders();

	void add_stream_line(std::vector<LineSegment>&, const vec2d&);
	void draw_lines();

private:
	Grid<vec2d> u_todraw = Grid<vec2d>(wp, hp);
	Grid<bool> solid = Grid<bool>(wp, hp);
};

GLAreaWindow::GLAreaWindow()
{
	set_title("brandy0");
	set_default_size(1280, 720);
	set_border_width(12);

	gl_area.set_hexpand(true);
	gl_area.set_vexpand(true);
	gl_area.set_auto_render(true);

	add(gl_area);

	gl_area.signal_realize().connect(sigc::mem_fun(*this, &GLAreaWindow::realize), true);
	gl_area.signal_unrealize().connect(sigc::mem_fun(*this, &GLAreaWindow::unrealize), false);
	gl_area.signal_render().connect(sigc::mem_fun(*this, &GLAreaWindow::render));

	show_all();
}

GLAreaWindow::~GLAreaWindow()
{
}

void GLAreaWindow::set_u(const Grid<vec2d>& u)
{
	u_todraw = u;
}

void GLAreaWindow::set_solid(const Grid<bool>& sld)
{
	solid = sld;
}

void GLAreaWindow::realize()
{
	gl_area.make_current();
	try
	{
		gl_area.throw_if_error();

		init_buffers();
		init_shaders();

		gl_context = gl_area.get_context();
	}
	catch (const Gdk::GLError& gle)
	{
		cerr << "An error occured making the context current during realize:" << endl;
		cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
	}
}

void GLAreaWindow::unrealize()
{
	gl_area.make_current();

	if (gl_context)
		gl_context->make_current();

	try
	{
		gl_area.throw_if_error();

		// delete buffers
	}
	catch (const Gdk::GLError& gle)
	{
		cerr << "An error occured making the context current during unrealize" << endl;
		cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
	}
}

bool GLAreaWindow::render(const Glib::RefPtr<Gdk::GLContext>& /* context */)
{
	glarea_w = gl_area.get_width();
	glarea_h = gl_area.get_height();

	cout << "now render" << endl;
	try
	{
		gl_area.throw_if_error();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_lines();

		glFlush();

		return true;
	}
	catch (const Gdk::GLError& gle)
	{
		cerr << "An error occurred in the render callback of the GLArea" << endl;
		cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
		return false;
	}
}

void GLAreaWindow::init_buffers()
{
	glGenVertexArrays(1, &gl_vao);
	glBindVertexArray(gl_vao);

	glGenBuffers(1, &gl_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gl_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static GLuint create_shader(int type, const char *src)
{
	auto shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		int log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

		std::string log_space(log_len + 1, ' ');
		glGetShaderInfoLog(shader, log_len, nullptr, (GLchar*)log_space.c_str());

		cerr << "Compile failure in " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << log_space << endl;

		glDeleteShader(shader);
	}

	return shader;
}

void GLAreaWindow::init_shaders()
{
	const std::string vertex_path = "/shaders/main.vs.glsl";
	const std::string fragment_path = "/shaders/main.fs.glsl";

	auto vshader_bytes = Gio::Resource::lookup_data_global(vertex_path);
	if (!vshader_bytes)
	{
		cerr << "Failed fetching vertex shader resource" << endl;
		gl_program = 0;
		return;
	}
	gsize vshader_size {vshader_bytes->get_size()};
	auto vshader = create_shader(GL_VERTEX_SHADER, (const char*)vshader_bytes->get_data(vshader_size));

	if (vshader == 0)
	{
		gl_program = 0;
		return;
	}

	auto fshader_bytes = Gio::Resource::lookup_data_global(fragment_path);
	if (!fshader_bytes)
	{
		cerr << "Failed fetching fragment shader resource" << endl;
		gl_program = 0;
		return;
	}
	gsize fshader_size {fshader_bytes->get_size()};
	auto fshader = create_shader(GL_FRAGMENT_SHADER, (const char*)fshader_bytes->get_data(fshader_size));

	if (fshader == 0)
	{
		glDeleteShader(vshader);
		gl_program = 0;
		return;
	}

	gl_program = glCreateProgram();
	glAttachShader(gl_program, vshader);
	glAttachShader(gl_program, fshader);

	glLinkProgram(gl_program);

	int status;
	glGetProgramiv(gl_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		int log_len;
		glGetProgramiv(gl_program, GL_INFO_LOG_LENGTH, &log_len);

		std::string log_space(log_len + 1, ' ');

		glGetProgramInfoLog(gl_program, log_len, nullptr, (GLchar*)log_space.c_str());

		cerr << "Linking failure: " << log_space << endl;

		glDeleteProgram(gl_program);
		gl_program = 0;
	}
	else
	{
		gl_mat = glGetUniformLocation(gl_program, "mat");
		
		glDetachShader(gl_program, vshader);
		glDetachShader(gl_program, fshader);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

Point to_poi(const vec2d& v)
{
	const double relx = v.x / w;
	const double rely = v.y / h;
	const Point res(round(relx * (wp - 1)), round(rely * (hp - 1)));
	return res;
}

void GLAreaWindow::add_stream_line(std::vector<LineSegment>& segs, const vec2d& ini)
{
	Point ipoi = to_poi(ini);
	if (!ipoi.inside(0, 0, wp - 1, hp - 1) || solid(ipoi))
		return;

	constexpr double step_size = .005;
	constexpr double max_steps = 3000;

	vec2d nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d u = u_todraw(cpoi);
		if (u.is_zero())
			break;
		const vec2d nnxt = nxt + step_size * u.get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, wp - 1, hp - 1) || solid(npoi))
			break;
		segs.push_back(LineSegment(nxt, nnxt));
		nxt = nnxt;
	}

	nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d nnxt = nxt - step_size * u_todraw(cpoi).get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, wp - 1, hp - 1) || solid(npoi))
			break;
		segs.push_back(LineSegment(nxt, nnxt));
		nxt = nnxt;
	}
}

static void compute_mat(float *mat)
{
	mat[0] = 2 / w;	mat[4] = 0;		mat[8] = 0;		mat[12] = -1;
	mat[1] = 0;		mat[5] = 2 / h;	mat[9] = 0;		mat[13] = -1;
	mat[2] = 0;		mat[6] = 0;		mat[10] = 1;	mat[14] = 0;
	mat[3] = 0;		mat[7] = 0;		mat[11] = 0;	mat[15] = 1;
	cout << "gla width = " << glarea_w << ", gla height = " << glarea_h << endl;
	float mlt = float(glarea_h) * w / (float(glarea_w) * h);
	if (mlt < 1)
	{
		mat[0] *= mlt;
		mat[12] *= mlt;
	}
	else
	{
		mat[1] /= mlt;
		mat[13] /= mlt;
	}
}

void GLAreaWindow::draw_lines()
{
	constexpr double line_d = .202;

	std::vector<LineSegment> segs;

	for (double x = w / 2; x >= 0; x -= line_d)
	{
		for (double y = h / 2; y >= 0; y -= line_d)
			add_stream_line(segs, vec2d(x, y));
		for (double y = h / 2 + line_d; y <= h; y += line_d)
			add_stream_line(segs, vec2d(x, y));
	}
	for (double x = w / 2 + line_d; x <= w; x += line_d)
	{
		for (double y = h / 2; y >= 0; y -= line_d)
			add_stream_line(segs, vec2d(x, y));
		for (double y = h / 2 + line_d; y <= h; y += line_d)
			add_stream_line(segs, vec2d(x, y));
	}

	GLfloat *vertex_data = new GLfloat[segs.size() * 8];

	for (int i = 0; i < segs.size(); i++)
	{
		vertex_data[i * 8] = segs[i].p0.x;
		vertex_data[i * 8 + 1] = segs[i].p0.y;
		vertex_data[i * 8 + 2] = 0.0f;
		vertex_data[i * 8 + 3] = 1.0f;
		vertex_data[i * 8 + 4] = segs[i].p1.x;
		vertex_data[i * 8 + 5] = segs[i].p1.y;
		vertex_data[i * 8 + 6] = 0.0f;
		vertex_data[i * 8 + 7] = 1.0f;
	}

	float mat[16];

	compute_mat(mat);

	glUseProgram(gl_program);

	cout << gl_program << endl;

	glUniformMatrix4fv(gl_mat, 1, GL_FALSE, mat);

	glBindBuffer(GL_ARRAY_BUFFER, gl_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * segs.size() * 8, vertex_data, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cout << "drawing " << segs.size() << " segments" << endl;
	cout << segs[10].p0 << segs[10].p1 << endl;

	glDrawArrays(GL_LINES, 0, segs.size() * 2);
}

void run()
{
	// ---
	// mock
	cout << "brandy0 started" << endl;
	w = h = 1;
	wp = hp = 32;
	Grid<bool> solid(w, h);
	solid.set_all(false);
	SimulatorParams params(w, h, wp, hp, .001,
			BoundaryCond(vec2d(1, 0), PressureBoundaryCond::DIRICHLET),
			BoundaryCond(vec2d(1, 0), PressureBoundaryCond::DIRICHLET),
			BoundaryCond(vec2d(0, 0), PressureBoundaryCond::VON_NEUMANN),
			BoundaryCond(vec2d(0, 0), PressureBoundaryCond::VON_NEUMANN),
			1, .1, solid);
	Simulator s(params);
	s.iter();

	auto app = Gtk::Application::create("");

	GLAreaWindow win;
	win.set_solid(solid);
	win.set_u(s.u1);

	app->run(win);
	// ---
}

}

int main()
{
	brandy0::run();
	return EXIT_SUCCESS;
}
