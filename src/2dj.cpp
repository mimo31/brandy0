#include <iostream>
//#include <eigen3/Eigen/Sparse>
//#include <eigen3/Eigen/IterativeLinearSolvers>
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

constexpr int32_t n = 784;

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

constexpr int32_t steps = 16;
constexpr double dx = 3 / 512.0;
constexpr double dt = .02;
constexpr double c0 = .003;
constexpr double c1 = .0001;
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

double getsig1(int32_t x, int32_t y)
{
    if (x < 0)
        x = 0;
    else if (x >= n)
        x = n - 1;
    if (y < 0)
        y = 0;
    else if (y >= n)
        y = n - 1;
    return sig1(x, y);
}

vec2d getjj1(const int32_t x, const int32_t y)
{
    const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
    const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
    return vec2d(x != clx ? 0 : jj1(clx, cly).x, y != cly ? 0 : jj1(clx, cly).y);
}

void init()
{
    for (int32_t x = 0; x < n; x++)
    {
        for (int32_t y = 0; y < n; y++)
        {
            const vec2d cor(x / double(n) - .5, y / double(n) - .5);
            //sig0[x + n * y] = sig1[x + n * y] = ((corx - .5) * (corx - .5) + (cory - .5) * (cory - .5) < .2) ? .8 : 0;
            sig0(x, y) = sig1(x, y) = 1.5 * exp(-cor.len2() * 128) + sigenv;
            jj0(x, y) = jj1(x, y) = vec2d(0, jenv);
            //jj0[x + n * y].y = jj1[x + n * y].x = jj1[x + n * y].y = 0;
        }
        //sig0[i] = sig1[i] = (cor >= .25 && cor < .75) ? 1 : 0;
        //sig[i] = (0.5 - abs(cor - 0.5)) / .5;
        //sig0[i] = sig1[i] = sin(M_PI * 2 * cor);
    }
}

void iter()
{
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            const double c = c0; //(x < n / 2 && 7 * n / 16 <= x) ? c1 : c0;
            /*const vec2d jxd(getjj1(x + 1, y).x - getjj1(x - 1, y).x, getjj1(x, y + 1).x - getjj1(x, y - 1).x);
            const vec2d jyd(getjj1(x + 1, y).y - getjj1(x - 1, y).y, getjj1(x, y + 1).y - getjj1(x, y - 1).y);
            const vec2d sigd(getsig1(x + 1, y) - getsig1(x - 1, y), getsig1(x, y + 1) - getsig1(x, y - 1));*/
            const vec2d jxd(
                (-getjj1(x + 2, y).x + 8 * getjj1(x + 1, y).x - 8 * getjj1(x - 1, y).x + getjj1(x - 2, y).x) / 6,
                (-getjj1(x, y + 2).x + 8 * getjj1(x, y + 1).x - 8 * getjj1(x, y - 1).x + getjj1(x, y - 2).x) / 6);
            const vec2d jyd(
                (-getjj1(x + 2, y).y + 8 * getjj1(x + 1, y).y - 8 * getjj1(x - 1, y).y + getjj1(x - 2, y).y) / 6,
                (-getjj1(x, y + 2).y + 8 * getjj1(x, y + 1).y - 8 * getjj1(x, y - 1).y + getjj1(x, y - 2).y) / 6);
            const vec2d sigd(
                (-getsig1(x + 2, y) + 8 * getsig1(x + 1, y) - 8 * getsig1(x - 1, y) + getsig1(x - 2, y)) / 6,
                (-getsig1(x, y + 2) + 8 * getsig1(x, y + 1) - 8 * getsig1(x, y - 1) + getsig1(x, y - 2)) / 6
            );
            const vec2d j = jj1(x, y);
            const double s = sig1(x, y);
            /*if (x == 0)
            {
                jxd.x = jj1[x + 1 + n * y].x;
                jyd.x = 0;
                sigd.x = sig1[x + 1 + n * y] - sig1[x + n * y];
            }
            else if (x == n - 1)
            {
                jxd.x = -jj1[x - 1 + n * y].x;
                jyd.x = 0;
                sigd.x = sig1[x + n * y] - sig1[x - 1 + n * y];
            }
            else
            {
                jxd.x = jj1[x + 1 + n * y].x - jj1[x - 1 + n * y].x;
                jyd.x = jj1[x + 1 + n * y].y - jj1[x - 1 + n * y].y;
                sigd.x = sig1[x + 1 + n * y] - sig1[x - 1 + n * y];
            }
            if (y == 0)
            {
                jxd.y = 0;
                jyd.y = jj1[x + n * (y + 1)].y - jenv;
                sigd.y = sig1[x + n * (y + 1)] - sig1[x + n * y];
            }
            else if (y == n - 1)
            {
                jxd.y = 0;
                jyd.y = jenv - jj1[x + n * (y - 1)].y;
                sigd.y = sig1[x + n * y] - sig1[x + n * (y - 1)];
            }
            else
            {
                jxd.y = jj1[x + n * (y + 1)].x - jj1[x + n * (y - 1)].x;
                jyd.y = jj1[x + n * (y + 1)].y - jj1[x + n * (y - 1)].y;
                sigd.y = sig1[x + n * (y + 1)] - sig1[x + n * (y - 1)];
            }*/
            nextsig(x, y) = -dt / dx * (jxd.x + jyd.y + j.dot(sigd) / s) + sig0(x, y);
            nextjj(x, y).x = -dt / dx * (c * c * sigd.x + j.dot(jxd) / s) + jj0(x, y).x;
            nextjj(x, y).y = -dt / dx * (c * c * sigd.y + j.dot(jyd) / s) + jj0(x, y).y;
        }
    }
    swap(sig1, nextsig);
    swap(sig0, nextsig);
    swap(jj1, nextjj);
    swap(jj0, nextjj);
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
    CImg<unsigned char> img(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = (sig1(x, y) + .2f) * 127;
            img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = clamp(val);
            /*const int val0 = (jj1[x + y * n].x * 500 + .5) * 127;
            const int val1 = (jj1[x + y * n].y * 500 + .5) * 127;
            img(x, y, 0) = val0;
            img(x, y, 1) = val1;
            img(x, y, 2) = 0;*/
        }
    }
    /*for (uint32_t i = 0; i < n / mf; i++)
    {
        double sm = 0;
        for (uint32_t j = 0; j < mf; j++)
            sm += sig1[mf * i + j];
        const int32_t c = (sm / mf / 2.5 + .5f) * 255;
        for (uint32_t k = 0; k < mf; k++)
        {
        for (uint32_t j = 256 - c; j < 256; j++)
        {
            img(i * mf + k, j, 0) = img(i * mf + k, j, 1) = img(i * mf + k, j, 2) = 255;
        }
        }
    }*/
    img.save(name.c_str());
}

void stats(const uint32_t iter)
{
    double total = 0;
    for (uint32_t i = 0; i < n * n; i++)
    {
        total += sig1.data[i];
    }
    /*double noise = 0;
    for (uint32_t i = 0; i < n - 1; i++)
    {
        noise += abs(sig1[i] - sig1[i + 1]);
    }*/
    cout << iter << ": " << total << /*", " << noise << */ endl;
}

/*void pushEntry(vector<Eigen::Triplet<double>> &vec, const int eq, const int var, const double val)
{
    vec.push_back(Eigen::Triplet<double>(eq, var, val));
}

void save(const Eigen::VectorXd& data, const uint32_t startind, const string& name)
{
    CImg<unsigned char> img(n, n, 1, 3, 0);
    for (uint32_t i = 0; i < n; i++)
    {
        for (uint32_t j = 0; j < 256; j++)
        {
            img(i, j, 0) = img(i, j, 1) = img(i, j, 2) = int(data[startind + i] * 127);
        }
    }
    img.save(name.c_str());
}*/

int main()
{
    /*uint32_t threads = 6;
    omp_set_num_threads(threads);
    Eigen::setNbThreads(threads);*/
    init();
    /*ofstream ofs("sol");
    ofs << fixed;
    ofs << setprecision(2);*/
    for (uint32_t i = 0; i < 1500; i++)
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
    /*constexpr double rtm = .01;
    double *sig0 = new double[n];
    for (int32_t i = 0; i < n; i++)
    {
        const double cor = i / double(n);
        //sig0[i] = (0.5 - abs(cor - 0.5)) / .5;
        sig0[i] = .5 + .5 * sin(M_PI * 2 * cor);//((cor >= .25 && cor < .75) ? 1 : 0;
    }
    vector<Eigen::Triplet<double>> nonzeros;
    for (uint32_t i = 0; i < n; i++)
    {
        pushEntry(nonzeros, i, i, 1);
    }
    constexpr double dtdx = dt * dt / (dx * dx);
    constexpr double mul = .003;//dtdx * rtm;
    
    for (uint32_t t = 1; t < steps - 1; t++)
    {
        pushEntry(nonzeros, t * n, t * n + 1, mul);
        pushEntry(nonzeros, t * n, t * n, -mul + 2);
        pushEntry(nonzeros, t * n, (t - 1) * n, -1);
        pushEntry(nonzeros, t * n, (t + 1) * n, -1);
        for (uint32_t x = 1; x < n - 1; x++)
        {
            pushEntry(nonzeros, t * n + x, t * n + x - 1, mul);
            pushEntry(nonzeros, t * n + x, t * n + x + 1, mul);
            pushEntry(nonzeros, t * n + x, t * n + x, -2 * mul + 2);
            pushEntry(nonzeros, t * n + x, (t - 1) * n + x, -1);
            pushEntry(nonzeros, t * n + x, (t + 1) * n + x, -1);
        }
        pushEntry(nonzeros, t * n + n - 1, t * n + n - 2, mul);
        pushEntry(nonzeros, t * n + n - 1, t * n + n - 1, -mul + 2);
        pushEntry(nonzeros, t * n + n - 1, (t - 1) * n + n - 1, -1);
        pushEntry(nonzeros, t * n + n - 1, (t + 1) * n + n - 1, -1);
    }
    pushEntry(nonzeros, (steps - 1) * n, (steps - 1) * n + 1, mul);
    pushEntry(nonzeros, (steps - 1) * n, (steps - 1) * n, -mul + 1);
    pushEntry(nonzeros, (steps - 1) * n, (steps - 2) * n, -1);
    for (uint32_t x = 1; x < n - 1; x++)
    {
        pushEntry(nonzeros, (steps - 1) * n + x, (steps - 1) * n + x - 1, mul);
        pushEntry(nonzeros, (steps - 1) * n + x, (steps - 1) * n + x + 1, mul);
        pushEntry(nonzeros, (steps - 1) * n + x, (steps - 1) * n + x, -2 * mul + 1);
        pushEntry(nonzeros, (steps - 1) * n + x, (steps - 2) * n + x, -1);
    }
    pushEntry(nonzeros, (steps - 1) * n + n - 1, (steps - 1) * n + n - 2, mul);
    pushEntry(nonzeros, (steps - 1) * n + n - 1, (steps - 1) * n + n - 1, -mul + 1);
    pushEntry(nonzeros, (steps - 1) * n + n - 1, (steps - 2) * n + n - 1, -1);
    constexpr uint32_t v = steps * n;
    Eigen::VectorXd b(v);
    Eigen::VectorXd sol(v);
    b.setZero();
    for (uint32_t i = 0; i < n; i++)
    {
        b[i] = sig0[i];
    }
    Eigen::SparseMatrix<double> mat(v, v);
    mat.setFromTriplets(nonzeros.begin(), nonzeros.end());
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    cout << "computing" << endl;
    solver.compute(mat);
    cout << "solving" << endl;
    sol = solver.solve(b);
    ofstream ofs("sol");
    ofs << fixed;
    ofs << setprecision(2);
    for (uint32_t i = 0; i < steps; i++)
    {
        save(sol, i * n, to_string(i) + ".png");
        for (uint32_t x = 0; x < n; x++)
        {
            ofs << sol[n * i + x] << " ";
        }
        ofs << '\n';
    }*/
    return EXIT_SUCCESS;
}