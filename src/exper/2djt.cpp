#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#include <omp.h>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

struct vec2d
{
    double x;
    double y;

    vec2d()
    {

    }

    vec2d(const double x, const double y) : x(x), y(y)
    {

    }

    double len2() const
    {
        return x * x + y * y;
    }

    vec2d operator+(const vec2d& other) const
    {
        return vec2d(x + other.x, y + other.y);
    }

    vec2d operator-(const vec2d& other) const
    {
        return vec2d(x - other.x, y - other.y);
    }

    vec2d operator*(const double a) const
    {
        return vec2d(a * x, a * y);
    }

    double dot(const vec2d& other) const
    {
        return x * other.x + y * other.y;
    }
};

constexpr int32_t n = 256;

template<typename T>
struct Grid
{
    T* data;

    Grid()
    {
        data = new T[n * n];
    }

    T& operator()(const uint32_t x, const uint32_t y)
    {
        return data[x + y * n];
    }
};

constexpr double dx = 3 / 512.0; // space step
constexpr double dt = .0003; // time step
constexpr double R = 1; // gas constant
constexpr double mm = 50; // molar mass
constexpr double c = 1; // molar thermal capacity
constexpr double k1 = 0.0001;//0.00001; // viscosity constant
constexpr double k2 = .001; // friction to heat constant
constexpr double k3 = .001; // heat conduction constant
constexpr double ks = 0; // stable distribution constant

//constexpr double jenv = -.001;
constexpr double sigenv = 1;
constexpr double venv = 0;
constexpr double jenv = sigenv * venv;

Grid<double> sig0;
Grid<double> sig1;
Grid<double> nextsig;
Grid<vec2d> jj0;
Grid<vec2d> jj1;
Grid<vec2d> nextjj;
Grid<double> t0;
Grid<double> t1;
Grid<double> nextt;

double getsig1(const int32_t x, const int32_t y)
{
    const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    return sig1(clx, cly);
}

vec2d getjj1(const int32_t x, const int32_t y)
{
    const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    return vec2d(x != clx ? 0 : jj1(clx, cly).x, y != cly ? 0 : jj1(clx, cly).y);
}

double gett1(const int32_t x, const int32_t y)
{
    const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    return t1(clx, cly);
}

void init()
{
    for (int32_t x = 0; x < n; x++)
    {
        for (int32_t y = 0; y < n; y++)
        {
            const vec2d cor(x / double(n) - .5, y / double(n) - .5);
            sig0(x, y) = sig1(x, y) = 1.5 * exp(-cor.len2() * 128) + sigenv;
            jj0(x, y) = jj1(x, y) = vec2d(0, 0);
            t0(x, y) = t1(x, y) = 1;
        }
    }
}

void iter()
{
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            const vec2d jxd(
                getjj1(x + 1, y).x - getjj1(x - 1, y).x,
                getjj1(x, y + 1).x - getjj1(x, y - 1).x);
            const vec2d jyd(
                getjj1(x + 1, y).y - getjj1(x - 1, y).y,
                getjj1(x, y + 1).y - getjj1(x, y - 1).y);
            const vec2d sigd(
                getsig1(x + 1, y) - getsig1(x - 1, y),
                getsig1(x, y + 1) - getsig1(x, y - 1)
            );
            const vec2d td(
                gett1(x + 1, y) - gett1(x - 1, y),
                gett1(x, y + 1) - gett1(x, y - 1)
            );
            const vec2d jlap(
                getjj1(x + 1, y).x + getjj1(x - 1, y).x + getjj1(x, y + 1).x + getjj1(x, y - 1).x - 4 * getjj1(x, y).x,
                getjj1(x + 1, y).y + getjj1(x - 1, y).y + getjj1(x, y + 1).y + getjj1(x, y - 1).y - 4 * getjj1(x, y).y
            );
            const double tlap = gett1(x + 1, y) + gett1(x - 1, y) + gett1(x, y + 1) + gett1(x, y - 1) - 4 * gett1(x, y);
            const double siglap = getsig1(x + 1, y) + getsig1(x - 1, y) + getsig1(x, y + 1) + getsig1(x, y - 1) - 4 * getsig1(x, y);
            const vec2d j = jj1(x, y);
            const double s = sig1(x, y);
            const double t = t1(x, y);
            nextsig(x, y) = -dt / dx * (jxd.x + jyd.y + j.dot(sigd) / s) + sig1(x, y) + dt / (dx * dx) * ks * siglap;
            nextjj(x, y).x = dt / dx * (-R / mm * (s * td.x + sigd.x * t) + k1 * jlap.x / dx - j.dot(jxd) / s) + jj1(x, y).x;
            nextjj(x, y).y = dt / dx * (-R / mm * (s * td.y + sigd.y * t) + k1 * jlap.y / dx - j.dot(jyd) / s) + jj1(x, y).y;
            nextt(x, y) = dt * (mm / c * k2 * k1 * sqrt(jlap.x * jlap.x + jlap.y * jlap.y) / (dx * dx) + k3 * tlap / (dx * dx) - j.dot(td) / s / dx) + t1(x, y);
            //nextjj(x, y).y = -dt / dx * (c * c * sigd.y + j.dot(jyd) / s) + jj0(x, y).y;
        }
    }
    swap(sig1, nextsig);
    swap(sig0, nextsig);
    swap(jj1, nextjj);
    swap(jj0, nextjj);
    swap(t1, nextt);
    swap(t0, nextt);
}

int32_t ind(const int32_t x)
{
    return min(max(x, 0), n - 1);
}

int clamp(const int val)
{
    return min(max(val, 0), 255);
}

void save(const string name)
{
    constexpr uint32_t mf = 1;
    CImg<unsigned char> simg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = (sig1(x, y) + .2f) * 127;
            simg(x, y, 0) = simg(x, y, 1) = simg(x, y, 2) = clamp(val);
        }
    }
    simg.save(name.c_str());
    CImg<unsigned char> timg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = (t1(x, y) - 1) * 4096 + 127;
            timg(x, y, 0) = timg(x, y, 1) = timg(x, y, 2) = clamp(val);
        }
    }
    timg.save(("t" + name).c_str());
}

void stats(const uint32_t iter)
{
    double total = 0;
    for (uint32_t i = 0; i < n * n; i++)
    {
        total += sig1.data[i];
    }
    cout << iter << ": " << total << /*", " << noise << */ endl;
}

int main()
{
    init();
    for (uint32_t i = 0; ; i++)
    {
        save(to_string(i) + ".png");
        stats(i);
        /*for (uint32_t x = 0; x < n; x++)
        {
            ofs << sig[x] << " ";
        }
        ofs << '\n';*/
        for (uint32_t j = 0; j < 100; j++)
        {
            iter();
        }
    }
    return EXIT_SUCCESS;
}