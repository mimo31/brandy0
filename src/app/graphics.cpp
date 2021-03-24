/**
 * graphics.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2021/02/21
 */
#include "graphics.hpp"

#include <functional>

#include <giomm/resource.h>

#include "display-modes.hpp"
#include "str.hpp"

namespace brandy0
{

void GraphicsManager::initBuffers()
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint createShader(int type, const char *src, const str& name)
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

		auto *log_space = new GLchar[log_len + 1];
		glGetShaderInfoLog(shader, log_len, nullptr, log_space);

		cerr << "Compile failure in " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << name << ": " << str(log_space) << endl;

		delete[] log_space;
		glDeleteShader(shader);
	}

	return shader;
}

struct UniformLoc
{
	str name;
	GLuint *pos;

	UniformLoc(const str& name, GLuint *const pos) : name(name), pos(pos)
	{
	}
};

GLuint loadProgram(const str& vshaderName, const str& fshaderName, const vec<UniformLoc>& uniforms)
{
	const str vertex_path = "/shaders/" + vshaderName + ".vs.glsl";
	const str fragment_path = "/shaders/" + fshaderName + ".fs.glsl";

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

		auto *log_space = new GLchar[log_len + 1];

		glGetProgramInfoLog(program, log_len, nullptr, log_space);

		cerr << "Linking failure: " << log_space << endl;

		delete[] log_space;
		glDeleteProgram(program);
		program = 0;
	}
	else
	{
		for (const UniformLoc &uloc : uniforms)
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

void GraphicsManager::initShaders()
{
	glWhiteProgram = loadProgram("plain", "white", { UniformLoc("mat", &glWhiteMat) });
	glPaintProgram = loadProgram("plain-float", "rainbow", { UniformLoc("mat", &glPaintMat) });
}

void GraphicsManager::init()
{
	if (!initialized)
	{
		initBuffers();
		initShaders();
		initialized = true;
	}
}

void GraphicsManager::destruct()
{
	ctx.reset();
}

FrameDrawer::FrameDrawer(const SimulationParams& p)
	: w(p.w), h(p.h), wp(p.wp), hp(p.hp), dx(p.get_dx()), dy(p.get_dy()), solid(Grid<bool>(wp, hp))
{
	p.shapeStack.set(solid);
}

void FrameDrawer::setFrontDisplayMode(const uint32_t fdm)
{
	frontDisplayMode = fdm;
}

void FrameDrawer::setBackDisplayMode(const uint32_t bdm)
{
	backDisplayMode = bdm;
}

Point FrameDrawer::to_poi(const double x, const double y) const
{
	const double relx = x / w;
	const double rely = y / h;
	const Point res(round(relx * (wp - 1)), round(rely * (hp - 1)));
	return res;
}

Point FrameDrawer::to_poi(const vec2d& v) const
{
	return to_poi(v.x, v.y);
}

vec2d FrameDrawer::to_coor(const uint32_t x, const uint32_t y) const
{
	return vec2d(x * w / double(wp - 1), y * h / double(hp - 1));
}

vec2d FrameDrawer::to_coor(const Point p) const
{
	return to_coor(p.x, p.y);
}

void FrameDrawer::computeMat(float *mat) const
{
	mat[0] = 2 / w;	mat[4] = 0;		mat[8] = 0;		mat[12] = -1;
	mat[1] = 0;		mat[5] = 2 / h;	mat[9] = 0;		mat[13] = -1;
	mat[2] = 0;		mat[6] = 0;		mat[10] = 1;	mat[14] = 0;
	mat[3] = 0;		mat[7] = 0;		mat[11] = 0;	mat[15] = 1;
	const float mlt = viewh * w / (vieww * h);
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

void FrameDrawer::addStreamLine(const SimFrame& frame, vec<LineSegment>& segs, const vec2d& ini)
{
	Point ipoi = to_poi(ini);
	if (!ipoi.inside(0, 0, wp - 1, hp - 1) || solid(ipoi))
		return;

	constexpr double step_size = .005;
	constexpr double max_steps = 300;

	vec2d nxt = ini;
	for (int i = 0; i < max_steps; i++)
	{
		const Point cpoi = to_poi(nxt);
		const vec2d u = frame.u(cpoi);
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
		const vec2d nnxt = nxt - step_size * frame.u(cpoi).get_unit();
		const Point npoi = to_poi(nnxt);
		if (!npoi.inside(0, 0, wp - 1, hp - 1) || solid(npoi))
			break;
		segs.push_back(LineSegment(nxt, nnxt));
		nxt = nnxt;
	}
}

void FrameDrawer::addArrow(const SimFrame& frame, vec<LineSegment>& segs, const vec2d& pos, const double norm_len)
{
	const double a = .004 * std::max(w, h);

	const Point poi = to_poi(pos);
	if (!poi.inside(0, 0, wp - 1, hp - 1))
		return;
	const vec2d u = frame.u(poi);
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
		const double mlt = .1 / norm_len;
		segs.push_back(LineSegment(pos, pos + u * mlt));
		const vec2d uu = u.get_unit();
		const vec2d av = uu * a;
		const vec2d avp = av.get_lrot();
		segs.push_back(LineSegment(pos + u * mlt - avp, pos + u * mlt + avp));
		segs.push_back(LineSegment(pos + u * mlt - avp, pos + u * mlt + av));
		segs.push_back(LineSegment(pos + u * mlt + avp, pos + u * mlt + av));
	}
}

void FrameDrawer::drawAll(const SimFrame& frame, GraphicsManager *const manager)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw back graphics
	if (backDisplayMode != BACK_DISPLAY_NONE)
	{
		std::function<double(uint32_t, uint32_t)> scfield = [this, frame](const uint32_t x, const uint32_t y){
			if (backDisplayMode == BACK_DISPLAY_VELOCITY_MAGNITUDE)
			{
				return frame.u(x, y).len();
			}
			if (backDisplayMode == BACK_DISPLAY_VELOCITY_CURL || backDisplayMode == BACK_DISPLAY_VELOCITY_RELATIVE_CURL)
			{
				// TODO: do not evaluate at solid points
				if (x == 0 || x == wp - 1 || y == 0 || y == hp - 1)
					return 0.0;
				const double curl = (frame.u(x + 1, y).y - frame.u(x - 1, y).y) / dx / 2 - (frame.u(x, y + 1).x - frame.u(x, y - 1).x) / dy / 2;
				if (backDisplayMode == BACK_DISPLAY_VELOCITY_CURL)
					return curl;
				const double vel = (4 * frame.u(x, y).len() + frame.u(x - 1, y).len() + frame.u(x + 1, y).len() + frame.u(x, y - 1).len() + frame.u(x, y + 1).len()) / 8;
				return vel == 0 ? 0.0 : curl / vel;
			}
			if (backDisplayMode == BACK_DISPLAY_PRESSURE)
			{
				return frame.p(x, y);
			}
			if (backDisplayMode == BACK_DISPLAY_VELOCITY_DIV)
			{
				// TODO: do not evaluate at solid points
				if (x == 0 || x == wp - 1 || y == 0 || y == hp - 1)
					return 0.0;
				const double div = (frame.u(x + 1, y).x - frame.u(x - 1, y).x) / dx / 2 + (frame.u(x, y + 1).y - frame.u(x, y - 1).y) / dy / 2;
				return div;
			}
			
			return 0.0;
		};

		double mn = scfield(0, 0);
		double mx = scfield(0, 0);
		for (uint32_t y = 0; y < hp; y++)
		{
			for (uint32_t x = 0; x < wp; x++)
			{
				const double val = scfield(x, y);
				mn = std::min(mn, val);
				mx = std::max(mx, val);
			}
		}
		if (mn != mx)
		{
			const uint32_t pointCount = (wp - 1) * (hp - 1) * 12;
			const uint32_t arsize = pointCount * 3;
			GLfloat *vertex_data = new GLfloat[arsize];
			for (uint32_t y = 0; y < hp - 1; y++)
			{
				for (uint32_t x = 0; x < wp - 1; x++)
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
					const uint32_t ind = 36 *  ((wp - 1) * y + x);
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
				}
			}

			float mat[16];

			computeMat(mat);

			glUseProgram(manager->glPaintProgram);

			glUniformMatrix4fv(manager->glPaintMat, 1, GL_FALSE, mat);
			
			glBindVertexArray(manager->glPaintVao);

			glBindBuffer(GL_ARRAY_BUFFER, manager->glPaintVbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * arsize, vertex_data, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			delete[] vertex_data;

			glDrawArrays(GL_TRIANGLES, 0, pointCount);
		}
	}

	// draw front graphics
	if (frontDisplayMode != FRONT_DISPLAY_NONE)
	{
		vec<LineSegment> segs;

		constexpr double line_d = .0147;

		const double max_ulen = sqrt(max<vec2d, double>(frame.u, [](const vec2d u){return u.len2();}));

		if (frontDisplayMode == FRONT_DISPLAY_VELOCITY_ARROWS)
		{
			const double spm = std::max(w, h);
			for (double x = w / 2; x < w; x += line_d * spm)
			{
				for (double y = h / 2; y < h; y += line_d * spm)
				{
					if (!solid(to_poi(x, y)))
						addArrow(frame, segs, vec2d(x, y), max_ulen);
				}
				for (double y = h / 2 - line_d * spm; y > 0; y -= line_d * spm)
				{
					if (!solid(to_poi(x, y)))
						addArrow(frame, segs, vec2d(x, y), max_ulen);
				}
			}
			for (double x = w / 2 - line_d * spm; x > 0; x -= line_d * spm)
			{
				for (double y = h / 2; y < h; y += line_d * spm)
				{
					if (!solid(to_poi(x, y)))
						addArrow(frame, segs, vec2d(x, y), max_ulen);
				}
				for (double y = h / 2 - line_d * spm; y > 0; y -= line_d * spm)
				{
					if (!solid(to_poi(x, y)))
						addArrow(frame, segs, vec2d(x, y), max_ulen);
				}
			}
		}
		else if (frontDisplayMode == FRONT_DISPLAY_VELOCITY_STREAMLINES)
		{
			for (double x = w / 2; x >= 0; x -= line_d)
			{
				for (double y = h / 2; y >= 0; y -= line_d)
					addStreamLine(frame, segs, vec2d(x, y));
				for (double y = h / 2 + line_d; y <= h; y += line_d)
					addStreamLine(frame, segs, vec2d(x, y));
			}
			for (double x = w / 2 + line_d; x <= w; x += line_d)
			{
				for (double y = h / 2; y >= 0; y -= line_d)
					addStreamLine(frame, segs, vec2d(x, y));
				for (double y = h / 2 + line_d; y <= h; y += line_d)
					addStreamLine(frame, segs, vec2d(x, y));
			}
		}

		GLfloat *vertex_data = new GLfloat[segs.size() * 4];

		for (uint32_t i = 0; i < segs.size(); i++)
		{
			vertex_data[i * 4] = segs[i].p0.x;
			vertex_data[i * 4 + 1] = segs[i].p0.y;
			vertex_data[i * 4 + 2] = segs[i].p1.x;
			vertex_data[i * 4 + 3] = segs[i].p1.y;
		}

		float mat[16];

		computeMat(mat);

		glUseProgram(manager->glWhiteProgram);

		glUniformMatrix4fv(manager->glWhiteMat, 1, GL_FALSE, mat);
	
		glBindVertexArray(manager->glWhiteVao);

		glBindBuffer(GL_ARRAY_BUFFER, manager->glWhiteVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * segs.size() * 4, vertex_data, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete[] vertex_data;

		glDrawArrays(GL_LINES, 0, segs.size() * 2);
	}

	glFlush();
}

void FrameDrawer::drawFrame(const SimFrame& frame, const double view_width, const double view_height, GraphicsManager *const manager)
{
	vieww = view_width;
	viewh = view_height;
	
	drawAll(frame, manager);
}

void FrameDrawer::generate_gl_structs()
{
	if (!has_gl_structs)
	{
		glGenFramebuffers(1, &gl_framebuf);
		glGenTextures(1, &gl_texture);
		has_gl_structs = true;
	}
}

void FrameDrawer::drawFrame(const SimFrame& frame, const uint32_t width, const uint32_t height, uint8_t *const data, const uint32_t linesize, GraphicsManager *const manager)
{
	manager->ctx->make_current();

	vieww = width;
	viewh = height;

	// get original state
	GLint origframebuf;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origframebuf);
	GLint origviewport[4];
	glGetIntegerv(GL_VIEWPORT, origviewport);

	generate_gl_structs();
	glBindFramebuffer(GL_FRAMEBUFFER, gl_framebuf);
	glBindTexture(GL_TEXTURE_2D, gl_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_texture, 0);
	glViewport(0, 0, width, height);
	drawAll(frame, manager);
	
	uint8_t *rawdata = new uint8_t[4 * width * height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawdata);
	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			data[y * linesize + 3 * x] = rawdata[4 * (y * width + x)];
			data[y * linesize + 3 * x + 1] = rawdata[4 * (y * width + x) + 1];
			data[y * linesize + 3 * x + 2] = rawdata[4 * (y * width + x) + 2];
		}
	}
	delete [] rawdata;

	// set back to original state
	glBindFramebuffer(GL_FRAMEBUFFER, origframebuf);
	glViewport(origviewport[0], origviewport[1], origviewport[2], origviewport[3]);
}

}