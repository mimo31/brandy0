#include "display-area.hpp"

#include <chrono>

#include "point.hpp"

namespace brandy0
{

using std::cout;
using std::cerr;
using std::endl;

DisplayArea::DisplayArea() : curFrame(nullptr)
{
	set_hexpand(true);
	set_vexpand(true);
	set_auto_render(true);

	signal_realize().connect(sigc::mem_fun(*this, &DisplayArea::realize), true);
	signal_unrealize().connect(sigc::mem_fun(*this, &DisplayArea::unrealize), false);
	signal_render().connect(sigc::mem_fun(*this, &DisplayArea::render));
}

bool DisplayArea::render(const Glib::RefPtr<Gdk::GLContext>& /* context */)
{
	//get_width();
	//get_height();
	make_current();

	if (glContext)
		glContext->make_current();

	try
	{
		throw_if_error();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		drawContent();

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

void reportError(const Gdk::GLError& gle, const std::string where)
{
	cerr << "make_current on glArea resulted in an error in " << where << endl;
	cerr << gle.domain() << "-" << gle.code() << "-" << gle.what() << endl;
}

void DisplayArea::realize()
{
	make_current();
	try
	{
		throw_if_error();

		initBuffers();
		initShaders();

		glContext = get_context();
	}
	catch (const Gdk::GLError& gle)
	{
		reportError(gle, "realize");
	}
}

void DisplayArea::unrealize()
{
	make_current();

	if (glContext)
		glContext->make_current();

	try
	{
		throw_if_error();

		// TODO resolve the below comment
		// delete buffers
	}
	catch (const Gdk::GLError& gle)
	{
		reportError(gle, "unrealize");
	}
}

void DisplayArea::initBuffers()
{
	glGenVertexArrays(1, &glVao);
	glBindVertexArray(glVao);

	glGenBuffers(1, &glVbo);
	glBindBuffer(GL_ARRAY_BUFFER, glVbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static GLuint createShader(int type, const char *src, const std::string& name)
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

		cerr << "Compile failure in " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << name << ": " << log_space << endl;

		glDeleteShader(shader);
	}

	return shader;
}

GLuint loadProgram(const std::string& vshaderName, const std::string& fshaderName, const UniformLocVec& uniforms)
{
	const std::string vertex_path = "/shaders/" + vshaderName + ".vs.glsl";
	const std::string fragment_path = "/shaders/" + fshaderName + ".fs.glsl";

	auto vshader_bytes = Gio::Resource::lookup_data_global(vertex_path);
	if (!vshader_bytes)
	{
		cerr << "Failed fetching vertex shader resource" << endl;
		return 0;
	}
	gsize vshader_size {vshader_bytes->get_size()};
	auto vshader = createShader(GL_VERTEX_SHADER, (const char*)vshader_bytes->get_data(vshader_size), vshaderName);

	if (vshader == 0)
	{
		return 0;
	}

	auto fshader_bytes = Gio::Resource::lookup_data_global(fragment_path);
	if (!fshader_bytes)
	{
		cerr << "Failed fetching fragment shader resource" << endl;
		return 0;
	}
	gsize fshader_size {fshader_bytes->get_size()};
	auto fshader = createShader(GL_FRAGMENT_SHADER, (const char*)fshader_bytes->get_data(fshader_size), fshaderName);

	if (fshader == 0)
	{
		glDeleteShader(vshader);
		return 0;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glLinkProgram(program);

	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		int log_len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

		std::string log_space(log_len + 1, ' ');

		glGetProgramInfoLog(program, log_len, nullptr, (GLchar*)log_space.c_str());

		cerr << "Linking failure: " << log_space << endl;

		glDeleteProgram(program);
		program = 0;
	}
	else
	{
		for (const UniformLoc uloc : uniforms)
		{
			*uloc.pos = glGetUniformLocation(program, uloc.name.c_str());
		}
		//glMat = glGetUniformLocation(program, "mat");
		
		glDetachShader(program, vshader);
		glDetachShader(program, fshader);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return program;
}

void DisplayArea::initShaders()
{
	glWhiteProgram = loadProgram("plain", "white", { UniformLoc("mat", &glWhiteMat) });
	glPaintProgram = loadProgram("plain-float", "rainbow", { UniformLoc("mat", &glPaintMat) });
}

Point DisplayArea::to_poi(const vec2d& v)
{
	const double relx = v.x / params->w;
	const double rely = v.y / params->h;
	const Point res(round(relx * (params->wp - 1)), round(rely * (params->hp - 1)));
	return res;
}

void DisplayArea::addStreamLine(std::vector<LineSegment>& segs, const vec2d& ini)
{
	Point ipoi = to_poi(ini);
	if (!ipoi.inside(0, 0, params->wp - 1, params->hp - 1) || params->solid(ipoi))
		return;

	constexpr double step_size = .005;
	constexpr double max_steps = 3000;

	vec2d nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d u = curFrame->u(cpoi);
		if (u.is_zero())
			break;
		const vec2d nnxt = nxt + step_size * u.get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, params->wp - 1, params->hp - 1) || params->solid(npoi))
			break;
		segs.push_back(LineSegment(nxt, nnxt));
		nxt = nnxt;
	}

	nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d nnxt = nxt - step_size * curFrame->u(cpoi).get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, params->wp - 1, params->hp - 1) || params->solid(npoi))
			break;
		segs.push_back(LineSegment(nxt, nnxt));
		nxt = nnxt;
	}
}

void DisplayArea::addArrow(std::vector<LineSegment>& segs, const vec2d& pos)
{
	constexpr double a = .004;

	const Point poi = to_poi(pos);
	if (!poi.inside(0, 0, params->wp - 1, params->hp - 1))
		return;
	const vec2d u = curFrame->u(poi);
	if (u.is_zero())
	{
		const vec2d v0 = pos + vec2d(a, a);
		const vec2d v1 = pos + vec2d(a, -a);
		const vec2d v2 = pos + vec2d(-a, -a);
		const vec2d v3 = pos + vec2d(-a, a);
		segs.push_back(LineSegment(v0, v1));
		segs.push_back(LineSegment(v1, v2));
		segs.push_back(LineSegment(v2, v3));
		segs.push_back(LineSegment(v3, v0));
	}
	else
	{
		const double mlt = .1;
		segs.push_back(LineSegment(pos, pos + u * mlt));
		const vec2d uu = u.get_unit();
		const vec2d av = uu * a;
		const vec2d avp = av.get_lrot();
		segs.push_back(LineSegment(pos + u * mlt - avp, pos + u * mlt + avp));
		segs.push_back(LineSegment(pos + u * mlt - avp, pos + u * mlt + av));
		segs.push_back(LineSegment(pos + u * mlt + avp, pos + u * mlt + av));
	}
}

void DisplayArea::computeMat(float *mat)
{
	mat[0] = 2 / params->w;	mat[4] = 0;		mat[8] = 0;		mat[12] = -1;
	mat[1] = 0;		mat[5] = 2 / params->h;	mat[9] = 0;		mat[13] = -1;
	mat[2] = 0;		mat[6] = 0;		mat[10] = 1;	mat[14] = 0;
	mat[3] = 0;		mat[7] = 0;		mat[11] = 0;	mat[15] = 1;
	float mlt = float(get_height()) * params->w / (float(get_width()) * params->h);
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

void DisplayArea::drawContent()
{
	if (curFrame == nullptr)
		return;
	
	constexpr double line_d = .0141;

	std::vector<LineSegment> segs;

	for (double x = params->w / 2; x < params->w; x += line_d)
	{
		for (double y = params->h / 2; y < params->h; y += line_d)
			addArrow(segs, vec2d(x, y));
		for (double y = params->h / 2 - line_d; y > 0; y -= line_d)
			addArrow(segs, vec2d(x, y));
	}
	for (double x = params->w / 2 - line_d; x > 0; x -= line_d)
	{
		for (double y = params->h / 2; y < params->h; y += line_d)
			addArrow(segs, vec2d(x, y));
		for (double y = params->h / 2 - line_d; y > 0; y -= line_d)
			addArrow(segs, vec2d(x, y));
	}

	/*
	for (double x = params->w / 2; x >= 0; x -= line_d)
	{
		for (double y = params->h / 2; y >= 0; y -= line_d)
			addStreamLine(segs, vec2d(x, y));
		for (double y = params->h / 2 + line_d; y <= params->h; y += line_d)
			addStreamLine(segs, vec2d(x, y));
	}
	for (double x = params->w / 2 + line_d; x <= params->w; x += line_d)
	{
		for (double y = params->h / 2; y >= 0; y -= line_d)
			addStreamLine(segs, vec2d(x, y));
		for (double y = params->h / 2 + line_d; y <= params->h; y += line_d)
			addStreamLine(segs, vec2d(x, y));
	}*/
	/*std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	for (int i = 0; i < ms.count() % 1000; i++)
	{
		const double y = i / 1000.0 * 2 - 1;
		segs.push_back(LineSegment(vec2d(-1, y), vec2d(1, y)));
	}*/

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

	computeMat(mat);

	glUseProgram(glWhiteProgram);

	glUniformMatrix4fv(glWhiteMat, 1, GL_FALSE, mat);

	glBindBuffer(GL_ARRAY_BUFFER, glVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * segs.size() * 8, vertex_data, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete [] vertex_data;

	glDrawArrays(GL_LINES, 0, segs.size() * 2);
}

void DisplayArea::setParams(const SimulatorParams *const params)
{
	this->params = params;
}

void DisplayArea::setCurFrame(const SimFrame& curFrame)
{
	if (this->curFrame == nullptr)
		this->curFrame = new SimFrame(curFrame);
	else
		*(this->curFrame) = curFrame;
}

void DisplayArea::redraw()
{
	queue_render();
}

}
