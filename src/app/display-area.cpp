#include "display-area.hpp"

#include <chrono>

#include "point.hpp"

namespace brandy0
{

DisplayArea::DisplayArea() : curFrame(nullptr), solid(0, 0)
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
	glGenVertexArrays(1, &glWhiteVao);
	glBindVertexArray(glWhiteVao);

	glGenBuffers(1, &glWhiteVbo);
	glBindBuffer(GL_ARRAY_BUFFER, glWhiteVbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenVertexArrays(1, &glPaintVao);
	glBindVertexArray(glPaintVao);

	glGenBuffers(1, &glPaintVbo);
	glBindBuffer(GL_ARRAY_BUFFER, glPaintVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

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

Point DisplayArea::to_poi(const double x, const double y)
{
	const double relx = x / params->w;
	const double rely = y / params->h;
	const Point res(round(relx * (params->wp - 1)), round(rely * (params->hp - 1)));
	return res;
}

Point DisplayArea::to_poi(const vec2d& v)
{
	return to_poi(v.x, v.y);
}

vec2d DisplayArea::to_coor(const uint32_t x, const uint32_t y)
{
	return vec2d(x * params->w / double(params->wp - 1), y * params->h / double(params->hp - 1));
}

vec2d DisplayArea::to_coor(const Point p)
{
	return to_coor(p.x, p.y);
}

void DisplayArea::addStreamLine(std::vector<LineSegment>& segs, const vec2d& ini)
{
	Point ipoi = to_poi(ini);
	if (!ipoi.inside(0, 0, params->wp - 1, params->hp - 1) || solid(ipoi))
		return;

	constexpr double step_size = .005;
	constexpr double max_steps = 300;

	vec2d nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d u = curFrame->u(cpoi);
		if (u.is_zero())
			break;
		const vec2d nnxt = nxt + step_size * u.get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, params->wp - 1, params->hp - 1) || solid(npoi))
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
		if (!npoi.inside(0, 0, params->wp - 1, params->hp - 1) || solid(npoi))
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
	const float mlt = float(get_height()) * params->w / (float(get_width()) * params->h);
	if (mlt < 1)
	{
		mat[0] *= mlt;
		mat[12] *= mlt;
	}
	else
	{
		mat[5] /= mlt;
		mat[13] /= mlt;
	}
}

void DisplayArea::drawContent()
{
	if (curFrame == nullptr)
		return;

	// draw back graphics
	if (backDisplayMode != BACK_DISPLAY_NONE)
	{
		std::function<double(uint32_t, uint32_t)> scfield = [this](const uint32_t x, const uint32_t y){
			if (backDisplayMode == BACK_DISPLAY_VELOCITY_MAGNITUDE)
			{
				return curFrame->u(x, y).len();
			}
			else if (backDisplayMode == BACK_DISPLAY_VELOCITY_CURL || backDisplayMode == BACK_DISPLAY_VELOCITY_RELATIVE_CURL)
			{
				if (x == 0 || x == params->wp - 1 || y == 0 || y == params->hp - 1)
					return 0.0;
				const double curl = (curFrame->u(x + 1, y).y - curFrame->u(x - 1, y).y) / params->get_dx() - (curFrame->u(x, y + 1).x - curFrame->u(x, y - 1).x) / params->get_dy();
				if (backDisplayMode == BACK_DISPLAY_VELOCITY_CURL)
					return curl;
				const double vel = (4 * curFrame->u(x, y).len() + curFrame->u(x - 1, y).len() + curFrame->u(x + 1, y).len() + curFrame->u(x, y - 1).len() + curFrame->u(x, y + 1).len()) / 8;
				return vel == 0 ? 0.0 : curl / vel;
			}
			else if (backDisplayMode == BACK_DISPLAY_PRESSURE)
			{
				return curFrame->p(x, y);
			}
			return 0.0;
		};

		double mn = scfield(0, 0);
		double mx = scfield(0, 0);
		for (uint32_t y = 0; y < params->hp; y++)
		{
			for (uint32_t x = 0; x < params->wp; x++)
			{
				const double val = scfield(x, y);
				mn = std::min(mn, val);
				mx = std::max(mx, val);
			}
		}
		if (mn != mx)
		{
			const uint32_t pointCount = (params->wp - 1) * (params->hp - 1) * 12;
			const uint32_t arsize = pointCount * 3;
			GLfloat *vertex_data = new GLfloat[arsize];
			for (uint32_t y = 0; y < params->hp - 1; y++)
			{
				for (uint32_t x = 0; x < params->wp - 1; x++)
				{
					const vec2d v00 = to_coor(x, y);
					const double c00 = (scfield(x, y) - mn) / (mx - mn);
					const vec2d v10 = to_coor(x + 1, y);
					const double c10 = (scfield(x + 1, y) - mn) / (mx - mn);
					const vec2d v01 = to_coor(x, y + 1);
					const double c01 = (scfield(x, y + 1) - mn) / (mx - mn);
					const vec2d v11 = to_coor(x + 1, y + 1);
					const double c11 = (scfield(x + 1, y + 1) - mn) / (mx - mn);
					const vec2d vcen = (v00 + v01 + v10 + v11) / 4;
					const double ccen = (c00 + c01 + c10 + c11) / 4;
					const uint32_t ind = 36 *  ((params->wp - 1) * y + x);
					vertex_data[ind + 9 * 0 + 3 * 0 + 0] = vcen.x;
					vertex_data[ind + 9 * 0 + 3 * 0 + 1] = vcen.y;
					vertex_data[ind + 9 * 0 + 3 * 0 + 2] = ccen;
					vertex_data[ind + 9 * 0 + 3 * 1 + 0] = v00.x;
					vertex_data[ind + 9 * 0 + 3 * 1 + 1] = v00.y;
					vertex_data[ind + 9 * 0 + 3 * 1 + 2] = c00;
					vertex_data[ind + 9 * 0 + 3 * 2 + 0] = v01.x;
					vertex_data[ind + 9 * 0 + 3 * 2 + 1] = v01.y;
					vertex_data[ind + 9 * 0 + 3 * 2 + 2] = c01;

					vertex_data[ind + 9 * 1 + 3 * 0 + 0] = vcen.x;
					vertex_data[ind + 9 * 1 + 3 * 0 + 1] = vcen.y;
					vertex_data[ind + 9 * 1 + 3 * 0 + 2] = ccen;
					vertex_data[ind + 9 * 1 + 3 * 1 + 0] = v01.x;
					vertex_data[ind + 9 * 1 + 3 * 1 + 1] = v01.y;
					vertex_data[ind + 9 * 1 + 3 * 1 + 2] = c01;
					vertex_data[ind + 9 * 1 + 3 * 2 + 0] = v11.x;
					vertex_data[ind + 9 * 1 + 3 * 2 + 1] = v11.y;
					vertex_data[ind + 9 * 1 + 3 * 2 + 2] = c11;

					vertex_data[ind + 9 * 2 + 3 * 0 + 0] = vcen.x;
					vertex_data[ind + 9 * 2 + 3 * 0 + 1] = vcen.y;
					vertex_data[ind + 9 * 2 + 3 * 0 + 2] = ccen;
					vertex_data[ind + 9 * 2 + 3 * 1 + 0] = v11.x;
					vertex_data[ind + 9 * 2 + 3 * 1 + 1] = v11.y;
					vertex_data[ind + 9 * 2 + 3 * 1 + 2] = c11;
					vertex_data[ind + 9 * 2 + 3 * 2 + 0] = v10.x;
					vertex_data[ind + 9 * 2 + 3 * 2 + 1] = v10.y;
					vertex_data[ind + 9 * 2 + 3 * 2 + 2] = c10;

					vertex_data[ind + 9 * 3 + 3 * 0 + 0] = vcen.x;
					vertex_data[ind + 9 * 3 + 3 * 0 + 1] = vcen.y;
					vertex_data[ind + 9 * 3 + 3 * 0 + 2] = ccen;
					vertex_data[ind + 9 * 3 + 3 * 1 + 0] = v10.x;
					vertex_data[ind + 9 * 3 + 3 * 1 + 1] = v10.y;
					vertex_data[ind + 9 * 3 + 3 * 1 + 2] = c10;
					vertex_data[ind + 9 * 3 + 3 * 2 + 0] = v00.x;
					vertex_data[ind + 9 * 3 + 3 * 2 + 1] = v00.y;
					vertex_data[ind + 9 * 3 + 3 * 2 + 2] = c00;
					/*vertex_data[ind + 3 * 0 + 0] = v00.x;
					vertex_data[ind + 3 * 0 + 1] = v00.y;
					vertex_data[ind + 3 * 0 + 2] = c00;
					vertex_data[ind + 3 * 1 + 0] = v01.x;
					vertex_data[ind + 3 * 1 + 1] = v01.y;
					vertex_data[ind + 3 * 1 + 2] = c01;
					vertex_data[ind + 3 * 2 + 0] = v10.x;
					vertex_data[ind + 3 * 2 + 1] = v10.y;
					vertex_data[ind + 3 * 2 + 2] = c10;
					vertex_data[ind + 3 * 3 + 0] = v11.x;
					vertex_data[ind + 3 * 3 + 1] = v11.y;
					vertex_data[ind + 3 * 3 + 2] = c11;
					vertex_data[ind + 3 * 4 + 0] = v01.x;
					vertex_data[ind + 3 * 4 + 1] = v01.y;
					vertex_data[ind + 3 * 4 + 2] = c01;
					vertex_data[ind + 3 * 5 + 0] = v10.x;
					vertex_data[ind + 3 * 5 + 1] = v10.y;
					vertex_data[ind + 3 * 5 + 2] = c10;*/
				}
			}

			float mat[16];

			computeMat(mat);

			glUseProgram(glPaintProgram);

			glUniformMatrix4fv(glPaintMat, 1, GL_FALSE, mat);
			
			glBindVertexArray(glPaintVao);

			glBindBuffer(GL_ARRAY_BUFFER, glPaintVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * arsize, vertex_data, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			delete[] vertex_data;

			glDrawArrays(GL_TRIANGLES, 0, pointCount);
		}
	}

	// draw front graphics
	if (frontDisplayMode != FRONT_DISPLAY_NONE)
	{

		std::vector<LineSegment> segs;

		constexpr double line_d = .0147;

		if (frontDisplayMode == FRONT_DISPLAY_VELOCITY_ARROWS)
		{
			for (double x = params->w / 2; x < params->w; x += line_d)
			{
				for (double y = params->h / 2; y < params->h; y += line_d)
				{
					if (!solid(to_poi(x, y)))
						addArrow(segs, vec2d(x, y));
				}
				for (double y = params->h / 2 - line_d; y > 0; y -= line_d)
				{
					if (!solid(to_poi(x, y)))
						addArrow(segs, vec2d(x, y));
				}
			}
			for (double x = params->w / 2 - line_d; x > 0; x -= line_d)
			{
				for (double y = params->h / 2; y < params->h; y += line_d)
				{
					if (!solid(to_poi(x, y)))
						addArrow(segs, vec2d(x, y));
				}
				for (double y = params->h / 2 - line_d; y > 0; y -= line_d)
				{
					if (!solid(to_poi(x, y)))
						addArrow(segs, vec2d(x, y));
				}
			}
		}

		if (frontDisplayMode == FRONT_DISPLAY_VELOCITY_STREAMLINES)
		{
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
			}
		}
		/*std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		for (int i = 0; i < ms.count() % 1000; i++)
		{
			const double y = i / 1000.0 * 2 - 1;
			segs.push_back(LineSegment(vec2d(-1, y), vec2d(1, y)));
		}*/

		GLfloat *vertex_data = new GLfloat[segs.size() * 4];

		for (int i = 0; i < segs.size(); i++)
		{
			vertex_data[i * 4] = segs[i].p0.x;
			vertex_data[i * 4 + 1] = segs[i].p0.y;
			vertex_data[i * 4 + 2] = segs[i].p1.x;
			vertex_data[i * 4 + 3] = segs[i].p1.y;
		}

		float mat[16];

		computeMat(mat);

		glUseProgram(glWhiteProgram);

		glUniformMatrix4fv(glWhiteMat, 1, GL_FALSE, mat);
	
		glBindVertexArray(glWhiteVao);

		glBindBuffer(GL_ARRAY_BUFFER, glWhiteVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * segs.size() * 4, vertex_data, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete[] vertex_data;

		glDrawArrays(GL_LINES, 0, segs.size() * 2);
	}
}

void DisplayArea::setParams(const SimulatorParams *const params)
{
	this->params = params;
	solid = Grid<bool>(params->wp, params->hp);
	params->shapeStack.set(solid);
}

void DisplayArea::setCurFrame(const SimFrame& curFrame)
{
	if (this->curFrame == nullptr)
		this->curFrame = new SimFrame(curFrame);
	else
		*(this->curFrame) = curFrame;
}

void DisplayArea::setBackDisplayMode(const uint32_t backMode)
{
	backDisplayMode = backMode;
}

void DisplayArea::setFrontDisplayMode(const uint32_t frontMode)
{
	frontDisplayMode = frontMode;
}

void DisplayArea::redraw()
{
	queue_render();
}

}
