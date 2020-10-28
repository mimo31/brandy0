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

    void operator+=(const vec2d &other)
    {
        x += other.x;
        y += other.y;
    }

    vec2d operator+(const vec2d &other) const
    {
        return vec2d(x + other.x, y + other.y);
    }

    vec2d operator-(const vec2d &other) const
    {
        return vec2d(x - other.x, y - other.y);
    }

    vec2d operator*(const double a) const
    {
        return vec2d(a * x, a * y);
    }

    double dot(const vec2d &other) const
    {
        return x * other.x + y * other.y;
    }
};

constexpr int32_t n = 1024;

constexpr uint32_t stepsperframe = 53 * 4;

constexpr double dx = 1 / 256.0; // space step
constexpr double dt = .00015;     // time step
constexpr double R = 1;          // gas constant
constexpr double mm = 50;        // molar mass
constexpr double c = 1;          // molar thermal capacity
constexpr double k1 = 0.003;    //0.00001; // viscosity constant
constexpr double k2 = 0;      // friction to heat constant
constexpr double k3 = .001;      // heat conduction constant
constexpr double ks = 2e-3;         // stable distribution constant

constexpr double tenv = 1;
constexpr double sigenv = 1;
constexpr double vin = .075;
constexpr double jin = sigenv * vin;

constexpr bool midcirc = true;
constexpr double midr = .05143;
constexpr int32_t safeto = int32_t((.5 - midr) * n) - 2;
constexpr int32_t safefrom = int32_t((.5 + midr) * n) + 3;

template <typename T>
struct Grid
{
    T *data;

    Grid()
    {
        data = new T[n * n];
    }

    T &operator()(const uint32_t x, const uint32_t y)
    {
        return data[x + y * n];
    }
};

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
    const double coorx = (x + .5) / n - .5;
    const double coory = (y + .5) / n - .5;
    const double len2 = coorx * coorx + coory * coory;
    if (len2 < midr * midr)
    {
        uint32_t tiles = 0;
        double sm = 0;
        if (coorx > 0 && (coorx + 1 / double(n)) * (coorx + 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += sig1(x + 1, y);
        }
        else if (coorx < 0 && (coorx - 1 / double(n)) * (coorx - 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += sig1(x - 1, y);
        }
        if (coory > 0 && coorx * coorx + (coory + 1 / double(n)) * (coory + 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += sig1(x, y + 1);
        }
        else if (coory < 0 && coorx * coorx + (coory - 1 / double(n)) * (coory - 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += sig1(x, y - 1);
        }
        return sm / tiles;
    }
    if (x < 0)
        return sigenv;
    if (x >= n)
        return sigenv;
    if (y < 0)
        return sigenv;
    if (y >= n)
        return sigenv;
    /*if (x < 0)
        return 2 * sig1(0, y) - sig1(1, y);
    if (x >= n)
        return 2 * sig1(n - 1, y) - sig1(n - 2, y);
    if (y < 0)
        return 2 * sig1(x, 0) - sig1(x, 1);
    if (y >= n)
        return 2 * sig1(x, n - 1) - sig1(x, n - 2);*/
    return sig1(x, y);
    //const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    //const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    //return sig1(clx, cly);
}

vec2d getjj1(const int32_t x, const int32_t y)
{
    const double coorx = (x + .5) / n - .5;
    const double coory = (y + .5) / n - .5;
    const double len2 = coorx * coorx + coory * coory;
    if (len2 < midr * midr)
    {
        uint32_t tiles = 0;
        vec2d sm(0, 0);
        if (coorx > 0 && (coorx + 1 / double(n)) * (coorx + 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += jj1(x + 1, y);
        }
        else if (coorx < 0 && (coorx - 1 / double(n)) * (coorx - 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += jj1(x - 1, y);
        }
        if (coory > 0 && coorx * coorx + (coory + 1 / double(n)) * (coory + 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += jj1(x, y + 1);
        }
        else if (coory < 0 && coorx * coorx + (coory - 1 / double(n)) * (coory - 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += jj1(x, y - 1);
        }
        const vec2d tang(-coory, coorx);
        return tang * (sm.dot(tang) / tang.dot(tang) / tiles);
    }
    if (x < 0)
        return vec2d(0, -jin);
    if (x >= n)
        return vec2d(0, -jin);
    if (y < 0)
        return vec2d(0, -jin);
    if (y >= n)
        return vec2d(0, -jin);
    /*if (x < 0)
        return jj1(0, y) * 2 - jj1(1, y);
    if (x >= n)
        return jj1(n - 1, y) * 2 - jj1(n - 2, y);
    if (y < 0)
        return jj1(x, 0) * 2 - jj1(x, 1);
    if (y >= n)
        return vec2d(0, -jin);*/
    return jj1(x, y);
}

double gett1(const int32_t x, const int32_t y)
{
    const double coorx = (x + .5) / n - .5;
    const double coory = (y + .5) / n - .5;
    const double len2 = coorx * coorx + coory * coory;
    if (len2 < midr * midr)
    {
        uint32_t tiles = 0;
        double sm = 0;
        if (coorx > 0 && (coorx + 1 / double(n)) * (coorx + 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += t1(x + 1, y);
        }
        else if (coorx < 0 && (coorx - 1 / double(n)) * (coorx - 1 / double(n)) + coory * coory >= midr * midr)
        {
            tiles++;
            sm += t1(x - 1, y);
        }
        if (coory > 0 && coorx * coorx + (coory + 1 / double(n)) * (coory + 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += t1(x, y + 1);
        }
        else if (coory < 0 && coorx * coorx + (coory - 1 / double(n)) * (coory - 1 / double(n)) >= midr * midr)
        {
            tiles++;
            sm += t1(x, y - 1);
        }
        return sm / tiles;
    }
    if (x < 0)
        return tenv;
    if (x >= n)
        return tenv;
    if (y < 0)
        return tenv;
    if (y >= n)
        return tenv;
    /*if (x < 0)
        return 2 * t1(0, y) - t1(1, y);
    if (x >= n)
        return 2 * t1(n - 1, y) - t1(n - 2, y);
    if (y < 0)
        return 2 * t1(x, 0) - t1(x, 1);
    if (y >= n)
        return 2 * t1(x, n - 1) - t1(x, n - 2);*/
    return t1(x, y);
    /*const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    return t1(clx, cly);*/
}

void init()
{
    for (int32_t x = 0; x < n; x++)
    {
        for (int32_t y = 0; y < n; y++)
        {
            const vec2d cor(x / double(n) - .5, y / double(n) - .5);
            sig0(x, y) = sig1(x, y) = /*1.5 * exp(-cor.len2() * 128) + */ sigenv;// + sigenv * .1 * exp((- (cor.x - .4) * (cor.x - .4) - cor.y * cor.y) * 32);
            jj0(x, y) = jj1(x, y) = vec2d(0, -jin);
            t0(x, y) = t1(x, y) = tenv;
        }
    }
}

void iter()
{
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            if (x >= 1 && x <= n - 2 && y >= 1 && y <= n - 2 && (x <= safeto || x >= safefrom || y <= safeto || y >= safefrom))
            {
                const vec2d jxd(
                    jj1(x + 1, y).x - jj1(x - 1, y).x,
                    jj1(x, y + 1).x - jj1(x, y - 1).x);
                const vec2d jyd(
                    jj1(x + 1, y).y - jj1(x - 1, y).y,
                    jj1(x, y + 1).y - jj1(x, y - 1).y);
                const vec2d sigd(
                    sig1(x + 1, y) - sig1(x - 1, y),
                    sig1(x, y + 1) - sig1(x, y - 1));
                const vec2d td(
                    t1(x + 1, y) - t1(x - 1, y),
                    t1(x, y + 1) - t1(x, y - 1));
                const vec2d jlap(
                    jj1(x + 1, y).x + jj1(x - 1, y).x + jj1(x, y + 1).x + jj1(x, y - 1).x - 4 * jj1(x, y).x,
                    jj1(x + 1, y).y + jj1(x - 1, y).y + jj1(x, y + 1).y + jj1(x, y - 1).y - 4 * jj1(x, y).y);
                const double tlap = t1(x + 1, y) + t1(x - 1, y) + t1(x, y + 1) + t1(x, y - 1) - 4 * t1(x, y);
                const double siglap = sig1(x + 1, y) + sig1(x - 1, y) + sig1(x, y + 1) + sig1(x, y - 1) - 4 * sig1(x, y);
                const vec2d j = jj1(x, y);
                const double s = sig1(x, y);
                const double t = t1(x, y);
                nextsig(x, y) = -dt / dx * (jxd.x + jyd.y + j.dot(sigd) / s) + sig1(x, y) + dt / (dx * dx) * ks * siglap;
                nextjj(x, y).x = dt / dx * (-R / mm * (s * td.x + sigd.x * t) + k1 * jlap.x / dx - j.dot(jxd) / s) + jj1(x, y).x;
                nextjj(x, y).y = dt / dx * (-R / mm * (s * td.y + sigd.y * t) + k1 * jlap.y / dx - j.dot(jyd) / s) + jj1(x, y).y;
                nextt(x, y) = dt * (mm / c * k2 * k1 * sqrt(jlap.x * jlap.x + jlap.y * jlap.y) / (dx * dx) + k3 * tlap / (dx * dx) - j.dot(td) / s / dx) + t1(x, y);
            }
            else
            {
                const double coorx = (x + .5) / n - .5;
                const double coory = (y + .5) / n - .5;
                const double len2 = coorx * coorx + coory * coory;
                if (len2 < midr * midr)
                    continue;
                //cout << "no skip" << endl;
                const vec2d jxd(
                    getjj1(x + 1, y).x - getjj1(x - 1, y).x,
                    getjj1(x, y + 1).x - getjj1(x, y - 1).x);
                const vec2d jyd(
                    getjj1(x + 1, y).y - getjj1(x - 1, y).y,
                    getjj1(x, y + 1).y - getjj1(x, y - 1).y);
                const vec2d sigd(
                    getsig1(x + 1, y) - getsig1(x - 1, y),
                    getsig1(x, y + 1) - getsig1(x, y - 1));
                const vec2d td(
                    gett1(x + 1, y) - gett1(x - 1, y),
                    gett1(x, y + 1) - gett1(x, y - 1));
                const vec2d jlap(
                    getjj1(x + 1, y).x + getjj1(x - 1, y).x + getjj1(x, y + 1).x + getjj1(x, y - 1).x - 4 * getjj1(x, y).x,
                    getjj1(x + 1, y).y + getjj1(x - 1, y).y + getjj1(x, y + 1).y + getjj1(x, y - 1).y - 4 * getjj1(x, y).y);
                const double tlap = gett1(x + 1, y) + gett1(x - 1, y) + gett1(x, y + 1) + gett1(x, y - 1) - 4 * gett1(x, y);
                const double siglap = getsig1(x + 1, y) + getsig1(x - 1, y) + getsig1(x, y + 1) + getsig1(x, y - 1) - 4 * getsig1(x, y);
                const vec2d j = jj1(x, y);
                const double s = sig1(x, y);
                const double t = t1(x, y);
                nextsig(x, y) = -dt / dx * (jxd.x + jyd.y + j.dot(sigd) / s) + sig1(x, y) + dt / (dx * dx) * ks * siglap;
                nextjj(x, y).x = dt / dx * (-R / mm * (s * td.x + sigd.x * t) + k1 * jlap.x / dx - j.dot(jxd) / s) + jj1(x, y).x;
                nextjj(x, y).y = dt / dx * (-R / mm * (s * td.y + sigd.y * t) + k1 * jlap.y / dx - j.dot(jyd) / s) + jj1(x, y).y;
                nextt(x, y) = dt * (mm / c * k2 * k1 * sqrt(jlap.x * jlap.x + jlap.y * jlap.y) / (dx * dx) + k3 * tlap / (dx * dx) - j.dot(td) / s / dx) + t1(x, y);
            }
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

uint32_t nclamp(const int32_t val)
{
    return min(max(val, 0), n - 1);
}

struct Col
{
    unsigned char r, g, b;

    constexpr Col(const unsigned char r, const unsigned char g, const unsigned char b) : r(r), g(g), b(b)
    {
    }
};

constexpr Col sp0(255, 255, 0);
constexpr Col sp1(0, 255, 0);

void setcol(CImg<unsigned char>& im, const Col& c, const uint32_t x, const uint32_t y)
{
    im(x, y, 0) = c.r;
    im(x, y, 1) = c.g;
    im(x, y, 2) = c.b;
}

void save(const string name)
{
    constexpr uint32_t mf = 1;
    CImg<unsigned char> simg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const double coorx = (x + .5) / n - .5;
            const double coory = (y + .5) / n - .5;
            const double len2 = coorx * coorx + coory * coory;
            if (len2 < midr * midr)
            {
                setcol(simg, (coorx < 0) ^ (coory < 0) ? sp0 : sp1, x, y);
                continue;
            }
            const int val = 127 + (sig1(x, y) - sigenv) * 2 * 127;
            simg(x, y, 0) = simg(x, y, 1) = simg(x, y, 2) = clamp(val);
        }
    }
    simg.save(name.c_str());
    CImg<unsigned char> timg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const double coorx = (x + .5) / n - .5;
            const double coory = (y + .5) / n - .5;
            const double len2 = coorx * coorx + coory * coory;
            if (len2 < midr * midr)
            {
                setcol(timg, (coorx < 0) ^ (coory < 0) ? sp0 : sp1, x, y);
                continue;
            }
            const int val = (t1(x, y) - 1) * 4096 + 127;
            timg(x, y, 0) = timg(x, y, 1) = timg(x, y, 2) = clamp(val);
        }
    }
    timg.save(("t" + name).c_str());
    CImg<unsigned char> jyimg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const double coorx = (x + .5) / n - .5;
            const double coory = (y + .5) / n - .5;
            const double len2 = coorx * coorx + coory * coory;
            if (len2 < midr * midr)
            {
                setcol(jyimg, (coorx < 0) ^ (coory < 0) ? sp0 : sp1, x, y);
                continue;
            }
            const int val = 127 - (jj1(x, y).y + jin) * 50 * 127;
            jyimg(x, y, 0) = jyimg(x, y, 1) = jyimg(x, y, 2) = clamp(val);
        }
    }
    jyimg.save(("jy" + name).c_str());
    CImg<unsigned char> fimg(n, n, 1, 3, 0);
    for (double sx = -.5; sx < .5; sx += 1 / 32.0)
    {
        double x = sx;
        double y = .499;
        uint32_t cou = 0;
        while (x > -.5 && x < .5 && y > -.5 && y < .5 && x * x + y * y >= midr * midr)
        {
            const uint32_t ix = nclamp(int32_t((x + .5) * n));
            const uint32_t iy = nclamp(int32_t((y + .5) * n));
            if (ix < 0 || iy < 0 || ix >= n || iy >= n)
                break;
            fimg(ix, iy, 0) = fimg(ix, iy, 1) = fimg(ix, iy, 2) = 255;
            constexpr double mul = .001;
            x += jj1(ix, iy).x * mul;
            y += jj1(ix, iy).y * mul;
            cou++;
            if (cou > 1000000)
                break;
        }
    }
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const double coorx = (x + .5) / n - .5;
            const double coory = (y + .5) / n - .5;
            const double len2 = coorx * coorx + coory * coory;
            if (len2 < midr * midr)
            {
                setcol(fimg, (coorx < 0) ^ (coory < 0) ? sp0 : sp1, x, y);
            }
        }
    }
    fimg.save(("f" + name).c_str());
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
    for (uint32_t i = 0;; i++)
    {
        save(to_string(i) + ".png");
        stats(i);
        /*for (uint32_t x = 0; x < n; x++)
        {
            ofs << sig[x] << " ";
        }
        ofs << '\n';*/
        for (uint32_t j = 0; j < stepsperframe; j++)
        {
            iter();
        }
    }
    return EXIT_SUCCESS;
}