/**
 * navier-stokes.cpp
 * 
 * Simulates fluid flow around an object by numerically solving the Navier-Stokes
 * equations using the Crank-Nicolson method
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/1
 */
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>

#include <omp.h>

#include "CImg.h"

#include "vec2d.h"

using namespace std;
using namespace cimg_library;

constexpr uint32_t stepsperframe = 53 * 4;
constexpr uint32_t itersperstep = 8;

constexpr double dx = 1 / 256.0; // space step
constexpr double dt = .00015;     // time step
constexpr double rho = 1;
constexpr double mu = 1;

constexpr double penv = 1;
constexpr vec2d uenv(0, -1);

constexpr bool midcirc = true;
constexpr double midr = .05143;
constexpr int32_t safeto = int32_t((.5 - midr) * n) - 2;
constexpr int32_t safefrom = int32_t((.5 + midr) * n) + 3;


struct State
{
    Grid<vec2d> u;
    Grid<double> p;
};

inline vec2d tocoor(const int32_t x, const int32_t y)
{
    return vec2d(x / double(n) - .5, y / double(n) - .5);
}

State s0, s1, s2;
Grid<bool> freepoints;
Grid<bool> safepoints;
Grid<uint32_t> pointnums;
uint32_t freecount = 0;

constexpr uint32_t CONST_VAR = UINT32_MAX;

enum Var
{
    Ux = 0,
    Uy = 1,
    P = 2
};

struct LinearVar
{
    double mult;
    uint32_t var;

    LinearVar(const double mult, const uint32_t var) : mult(mult), var(var)
    {
    }

    LinearVar(const double mult, const uint32_t x, const uint32_t y, const Var vtype) : mult(mult), var(pointnums(x, y) + vtype * freecount)
    {
    }
};

vector<LinearVar>* vardependence;

vector<LinearVar>& vdep(const int32_t x, const int32_t y, const Var vtype)
{
    return vardependence[x + 1 + (n + 2) * (y + 1) + (n + 2) * (n + 2) * vtype];
}

void init()
{
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            s0.u(x, y) = s1.u(x, y) = s2.u(x, y) = uenv;
            s0.p(x, y) = s1.p(x, y) = s2.p(x, y) = penv;
            freepoints(x, y) = tocoor(x, y).len2() < midr * midr;
            if (freepoints(x, y))
            {
                pointnums(x, y) = freecount;
                freecount++;
            }
        }
    }
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            if (x == 0 || x == n - 1 || y == 0 || y == n - 1)
                safepoints(x, y) = false;
            else
                safepoints(x, y) = freepoints(x, y) && freepoints(x + 1, y) && freepoints(x - 1, y) && freepoints(x, y + 1) && freepoints(x, y - 1);
        }
    }
    vardependence = new vector<LinearVar>[(n + 2) * (n + 2) * 3];
    for (int32_t cor = 0; cor < n; cor++)
    {
        vdep(cor, -1, Var::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(cor, n, Var::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(cor, -1, Var::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(cor, n, Var::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(cor, -1, Var::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(cor, n, Var::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(-1, cor, Var::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(n, cor, Var::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(-1, cor, Var::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(n, cor, Var::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(-1, cor, Var::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(n, cor, Var::P).push_back(LinearVar(penv, CONST_VAR));
    }
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            if (freepoints(x, y))
                continue;
            const int32_t movex = x < n / 2 ? -1 : 1;
            const int32_t movey = y < n / 2 ? -1 : 1;
            const bool mvxfree = freepoints(x + movex, y);
            const bool mvyfree = freepoints(x, y + movey);
            if (!mvxfree && !mvyfree)
                continue;
            const double fx = x / double(n) - .5;
            const double fy = y / double(n) - .5;
            const uint32_t pcou = mvxfree && mvyfree ? 2 : 1;
            if (mvxfree)
            {
                vdep(x, y, Var::Ux).push_back(LinearVar(fx * fx / (fx * fx + fy * fy) / pcou, x + movex, y, Var::Ux));
                vdep(x, y, Var::Ux).push_back(LinearVar(fx * fy / (fx * fx + fy * fy) / pcou, x + movex, y, Var::Uy));
                vdep(x, y, Var::Uy).push_back(LinearVar(fx * fy / (fx * fx + fy * fy) / pcou, x + movex, y, Var::Ux));
                vdep(x, y, Var::Uy).push_back(LinearVar(fy * fy / (fx * fx + fy * fy) / pcou, x + movex, y, Var::Uy));
                vdep(x, y, Var::P).push_back(LinearVar(1 / double(pcou), x + movex, y, Var::P));
            }
            if (mvyfree)
            {
                vdep(x, y, Var::Ux).push_back(LinearVar(fx * fx / (fx * fx + fy * fy) / pcou, x, y + movey, Var::Ux));
                vdep(x, y, Var::Ux).push_back(LinearVar(fx * fy / (fx * fx + fy * fy) / pcou, x, y + movey, Var::Uy));
                vdep(x, y, Var::Uy).push_back(LinearVar(fx * fy / (fx * fx + fy * fy) / pcou, x, y + movey, Var::Ux));
                vdep(x, y, Var::Uy).push_back(LinearVar(fy * fy / (fx * fx + fy * fy) / pcou, x, y + movey, Var::Uy));
                vdep(x, y, Var::P).push_back(LinearVar(1 / double(pcou), x + movex, y, Var::P));
            }
        }
    }
}

constexpr uint32_t vars = 3 * n * n;

vector<Eigen::Triplet<double>> nonzeros;
Eigen::VectorXd f(vars);
Eigen::VectorXd jacsol(vars);
Eigen::SparseMatrix<double, Eigen::RowMajor> jac(vars, vars);
Eigen::BiCGSTAB<Eigen::SparseMatrix<double, Eigen::RowMajor>> solver;
uint32_t nexteq;

void pushentry(const int eq, const int var, const double val)
{
    nonzeros.push_back(Eigen::Triplet<double>(eq, var, val));
}

void pushsafes()
{
    for (int32_t y = 1; y < n - 1; y++)
    {
        for (int32_t x = 1; x < n - 1; x++)
        {
            if (!safepoints(x, y))
                x = n - x;
            const uint32_t massconseq = pointnums(x, y) + 2 * freecount;
            pushentry(massconseq, pointnums(x + 1, y), 1);
            pushentry(massconseq, pointnums(x - 1, y), -1);
            pushentry(massconseq, pointnums(x, y + 1) + freecount, 1);
            pushentry(massconseq, pointnums(x, y - 1) + freecount, -1);
            f[massconseq] = s1.u(x + 1, y).x - s1.u(x - 1, y).x + s1.u(x, y + 1).y - s1.u(x, y - 1).y;
            const uint32_t xtimeeq = pointnums(x, y);
            pushentry(xtimeeq, pointnums(x, y), 2 * rho / dt + (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / (2 * dx) + 4 * mu / (dx * dx));
            pushentry(xtimeeq, pointnums(x - 1, y), -s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            pushentry(xtimeeq, pointnums(x + 1, y), s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            pushentry(xtimeeq, pointnums(x, y - 1), -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            pushentry(xtimeeq, pointnums(x, y + 1), s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            pushentry(xtimeeq, pointnums(x, y) + freecount, (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / (2 * dx));
            f[xtimeeq] = 2 * rho * (s1.u(x, y).x - s0.u(x, y).x) / dt // time derivative
                + s1.u(x, y).x * (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / (2 * dx) + s1.u(x, y).y * (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / (2 * dx) // t + 1: u dot grad u
                + (s1.p(x + 1, y) - s1.p(x - 1, y)) / (2 * dx) // t + 1: grad p
                - mu * (s1.u(x + 1, y).x + s1.u(x - 1, y).x + s1.u(x, y + 1).x + s1.u(x, y - 1).x - 4 * s1.u(x, y).x) / (dx * dx) // t + 1: mu lap u
                + s0.u(x, y).x * (s0.u(x + 1, y).x - s0.u(x - 1, y).x) / (2 * dx) + s0.u(x, y).y * (s0.u(x, y + 1).x - s0.u(x, y - 1).x) / (2 * dx) // t: u dot grad u
                + (s0.p(x + 1, y) - s0.p(x - 1, y)) / (2 * dx) // t: grad p
                - mu * (s0.u(x + 1, y).x + s0.u(x - 1, y).x + s0.u(x, y + 1).x + s0.u(x, y - 1).x - 4 * s0.u(x, y).x) / (dx * dx); // t: mu lap u
            const uint32_t ytimeeq = pointnums(x, y) + freecount;
            pushentry(ytimeeq, pointnums(x, y) + freecount, 2 * rho / dt + (s1.u(x, y + 1) - s1.u(x, y - 1)) / (2 * dx) + 4 * mu / (dx * dx));
            pushentry(ytimeeq, pointnums(x - 1, y) + freecount, -s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            pushentry(ytimeeq, pointnums(x + 1, y) + freecount, s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            pushentry(ytimeeq, pointnums(x, y - 1), -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            pushentry(ytimeeq, pointnums(x, y + 1), s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            pushentry(ytimeeq, pointnums(x, y), (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / (2 * dx));
            f[ytimeeq] = 2 * rho * (s1.u(x, y).y - s0.u(x, y).y) / dt // time derivative
                + s1.u(x, y).x * (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / (2 * dx) + s1.u(x, y).y * (s1.u(x, y + 1).y - s1.u(x, y - 1).y) / (2 * dx) // t + 1: u dot grad u
                + (s1.p(x, y + 1) - s1.p(x, y - 1)) / (2 * dx) // t + 1: grad p
                - mu * (s1.u(x + 1, y).y + s1.u(x - 1, y).y + s1.u(x, y + 1).y + s1.u(x, y - 1).y - 4 * s1.u(x, y).y) / (dx * dx) // t + 1: mu lap u
                + s0.u(x, y).x * (s0.u(x + 1, y).y - s0.u(x - 1, y).y) / (2 * dx) + s0.u(x, y).y * (s0.u(x, y + 1).y - s0.u(x, y - 1).y) / (2 * dx) // t: u dot grad u
                + (s0.p(x, y + 1) - s0.p(x, y - 1)) / (2 * dx) // t: grad p
                - mu * (s0.u(x + 1, y).y + s0.u(x - 1, y).y + s0.u(x, y + 1).y + s0.u(x, y - 1).y - 4 * s0.u(x, y).y) / (dx * dx); // t: mu lap u
        }
    }
}

struct PartialDer
{
    uint32_t var;
    double der;

    PartialDer(const uint32_t var, const double der) : var(var), der(der)
    {
    }
};

struct PartialDers
{
    vector<PartialDer> ders;

    void add(const PartialDer d)
    {
        for (uint32_t i = 0; i < ders.size(); i++)
        {
            if (ders[i].var == d.var)
            {
                ders[i].der += d.der;
                return;
            }
        }
        ders.push_back(d);
    }

    void add(const uint32_t var, const double der)
    {
        add(PartialDer(var, der));
    }
};

double varvalue(const uint32_t var)
{
    if (var == CONST_VAR)
        return 1;
    if (var < freecount)
        return s1.u.data[v]
}

void pushunsafes()
{
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            if (safepoints(x, y) || !freepoints(x, y))
                continue;
            const uint32_t massconseq = pointnums(x, y) + 2 * freecount;
            PartialDers massconsders;
            double uxxpy = 0;
            for (LinearVar lv : vdep(x + 1, y, Var::Ux))
            {
                massconsders.add(lv.var, lv.mult);
                uxxpy += lv.mult * 
            }
        }
    }
}

void iter()
{
    // let state 1 be the current best approximation of the next state (assumed)
    for (uint32_t i = 0; i < itersperstep; i++)
    {

    }
    // iterate a few times (improve the current best approximation):
    //// show
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {

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
        for (uint32_t j = 0; j < stepsperframe; j++)
        {
            iter();
        }
    }
    return EXIT_SUCCESS;
}