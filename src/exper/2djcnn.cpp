#include <iostream>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>

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

    vec2d operator+(const vec2d &other) const
    {
        return vec2d(x + other.x, y + other.y);
    }

    vec2d operator-(const vec2d &other) const
    {
        return vec2d(x - other.x, y - other.y);
    }

    vec2d &operator*=(const double a)
    {
        x *= a;
        y *= a;
        return *this;
    }

    friend vec2d operator*(vec2d vec, const double a)
    {
        vec *= a;
        return vec;
    }

    friend vec2d operator*(const double a, vec2d vec)
    {
        vec *= a;
        return vec;
    }

    double dot(const vec2d &other) const
    {
        return x * other.x + y * other.y;
    }
};

constexpr int32_t n = 512;
constexpr double dx = 3 / 512.0;
constexpr double maxdt = .04;
double dt = maxdt;
constexpr double c0 = .03;

template <typename T>
struct Grid
{
    T *data;

    Grid()
    {
        data = new T[n * n];
    }

    ~Grid()
    {
        delete[] data;
    }

    T &operator()(const uint32_t x, const uint32_t y) const
    {
        return data[x + y * n];
    }

    Grid &operator=(const Grid &other)
    {
        copy_n(other.data, n * n, data);
        return *this;
    }
};

struct State
{
    Grid<double> sig;
    Grid<vec2d> jj;

    State()
    {
    }

    double getsig(int32_t x, int32_t y) const
    {
        if (x < 0)
            x = 0;
        else if (x >= n)
            x = n - 1;
        if (y < 0)
            y = 0;
        else if (y >= n)
            y = n - 1;
        return sig(x, y);
    }

    vec2d getjj(const int32_t x, const int32_t y) const
    {
        const int32_t clx = x < 0 ? 0 : (x >= n ? n - 1 : x);
        const int32_t cly = y < 0 ? 0 : (y >= n ? n - 1 : y);
        return vec2d(x != clx ? 0 : jj(clx, cly).x, y != cly ? 0 : jj(clx, cly).y);
    }

    /*double getsigdt(const int32_t x, const int32_t y) const
    {
        return -(getjjcendiffx(x, y).x + getjjcendiffy(x, y).y) / 2 / dx - (jj(x, y).x * getsigcendiffx(x, y) + jj(x, y).y * getsigcendiffy(x, y)) / 2 / dx / sig(x, y);
    }

    vec2d getjjdt(const int32_t x, const int32_t y) const
    {
        return vec2d(-c0 * c0 * getsigcendiffx(x, y) / 2 / dx - (jj(x, y).x * getjjcendiffx(x, y).x + jj(x, y).y * getjjcendiffy(x, y).x) / 2 / dx / sig(x, y),
                     -c0 * c0 * getsigcendiffy(x, y) / 2 / dx - (jj(x, y).x * getjjcendiffx(x, y).y + jj(x, y).y * getjjcendiffy(x, y).y) / 2 / dx / sig(x, y));
    }*/

    double getsigcendiffx(const int32_t x, const int32_t y) const
    {
        return getsig(x + 1, y) - getsig(x - 1, y);
    }

    double getsigcendiffy(const int32_t x, const int32_t y) const
    {
        return getsig(x, y + 1) - getsig(x, y - 1);
    }

    vec2d getjjcendiffx(const int32_t x, const int32_t y) const
    {
        return getjj(x + 1, y) - getjj(x - 1, y);
    }

    vec2d getjjcendiffy(const int32_t x, const int32_t y) const
    {
        return getjj(x, y + 1) - getjj(x, y - 1);
    }
};

//constexpr double c1 = .0001;
//constexpr double jenv = -.001;
constexpr double sigenv = 1;
constexpr double venv = 0;
constexpr double jenv = sigenv * venv;

State stt;
State temp0;
State temp1;

void init()
{
    for (int32_t x = 0; x < n; x++)
    {
        for (int32_t y = 0; y < n; y++)
        {
            const vec2d cor(x / double(n) - .5, y / double(n) - .5);
            //sig0[x + n * y] = sig1[x + n * y] = ((corx - .5) * (corx - .5) + (cory - .5) * (cory - .5) < .2) ? .8 : 0;
            stt.sig(x, y) = temp0.sig(x, y) = temp1.sig(x, y) = 1.5 * exp(-cor.len2() * 128) + sigenv;
            stt.jj(x, y) = temp0.jj(x, y) = temp1.jj(x, y) = vec2d(0, jenv);
            //jj0[x + n * y].y = jj1[x + n * y].x = jj1[x + n * y].y = 0;
        }
        //sig0[i] = sig1[i] = (cor >= .25 && cor < .75) ? 1 : 0;
        //sig[i] = (0.5 - abs(cor - 0.5)) / .5;
        //sig0[i] = sig1[i] = sin(M_PI * 2 * cor);
    }
}

/*double newsig(const State &olds, const State &news, const int32_t x, const int32_t y)
{
    return olds.sig(x, y) + dt / 2 * (olds.getsigdt(x, y) + news.getsigdt(x, y));
}

vec2d newjj(const State &olds, const State &news, const int32_t x, const int32_t y)
{
    return olds.jj(x, y) + dt / 2 * (olds.getjjdt(x, y) + news.getjjdt(x, y));
}

double sigerror(const State &olds, const State &news)
{
    double total = 0;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            double diff = news.sig(x, y) - newsig(olds, news, x, y);
            diff *= 2 / dt * news.sig(x, y);
            total += diff * diff;
        }
    }
    return total;
}

double jjerror(const State &olds, const State &news)
{
    double total = 0;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            total += (news.jj(x, y) - newjj(olds, news, x, y)).len2() * 2 / dt * news.sig(x, y);
        }
    }
    return total;
}*/

constexpr uint32_t vars = 3 * n * n;

vector<Eigen::Triplet<double>> nonzeros;
Eigen::VectorXd f(vars);
Eigen::VectorXd jacsol(vars);
Eigen::SparseMatrix<double, Eigen::RowMajor> jac(vars, vars);
Eigen::BiCGSTAB<Eigen::SparseMatrix<double, Eigen::RowMajor>> solver;

/*bool toodifferent(const double a, const double b)
{
    constexpr double reltol = .01;
    constexpr double abstol = 1e-20;
    return a != b && abs(a - b) > reltol * (a + b) && abs(a - b) > abstol;
}

void checkf()
{
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            double diff = temp0.sig(x, y) - newsig(stt, temp0, x, y);
            diff *= 2 / dt * temp0.sig(x, y);
            const double dd = diff * diff;
            const double ff = f[x + y * n] * f[x + y * n];
            if (toodifferent(dd, ff))
            {
                cout << dd << " " << ff << endl;
                cout << "sig fail" << endl;
            }
        }
    }
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            vec2d diff = temp0.jj(x, y) - newjj(stt, temp0, x, y);
            diff *= 2 / dt * temp0.sig(x, y);
            const double ddx = diff.x * diff.x;
            const double ddy = diff.y * diff.y;
            const double expdify = 2 / dt * temp0.sig(x, y) * (temp0.jj(x, y).y - stt.jj(x, y).y - dt / 2 * (stt.getjjdt(x, y).y + temp0.getjjdt(x, y).y));
            const double expy = expdify * expdify;
            const double ffx = f[x + y * n + n * n] * f[x + y * n + n * n];
            const double ffy = f[x + y * n + 2 * n * n] * f[x + y * n + 2 * n * n];
            if (toodifferent(ddx, ffx))
            {
                cout << ddx << " " << ffx << endl;
                cout << "jx fail" << endl;
            }
            if (toodifferent(ddy, ffy))
            {
                cout << ddy << " " << ffy << endl;
                cout << "jy fail" << endl;
            }
            if (toodifferent(expy, ffy))
            {
                cout << "expy != ffy" << endl;
            }
            if (toodifferent(expy, ddy))
            {
                cout << expy << " " << ddy << endl;
                cout << "expy != ddy" << endl;
            }
        }
    }
}*/

void pushEntry(const int eq, const int var, const double val)
{
    nonzeros.push_back(Eigen::Triplet<double>(eq, var, val));
}

void pushsigeqentries(const int32_t x, const int32_t y)
{
    const int eq = x + y * n;
    double dsigxy = 4 / dt * temp0.sig(x, y) + (stt.getjjcendiffx(x, y).x + stt.getjjcendiffy(x, y).y) / 2 / dx + (stt.jj(x, y).x * stt.getsigcendiffx(x, y) + stt.jj(x, y).y * stt.getsigcendiffy(x, y)) / 2 / dx / stt.sig(x, y) - 2 * stt.sig(x, y) / dt + (temp0.getjjcendiffx(x, y).x + temp0.getjjcendiffy(x, y).y) / 2 / dx;
    const double dsigxpy = temp0.jj(x, y).x / 2 / dx;
    if (x == n - 1)
        dsigxy += dsigxpy;
    else
        pushEntry(eq, x + 1 + y * n, dsigxpy);
    const double dsigxmy = -temp0.jj(x, y).x / 2 / dx;
    if (x == 0)
        dsigxy += dsigxmy;
    else
        pushEntry(eq, x - 1 + y * n, dsigxmy);
    const double dsigxyp = temp0.jj(x, y).y / 2 / dx;
    if (y == n - 1)
        dsigxy += dsigxyp;
    else
        pushEntry(eq, x + (y + 1) * n, dsigxyp);
    const double dsigxym = -temp0.jj(x, y).y / 2 / dx;
    if (y == 0)
        dsigxy += dsigxym;
    else
        pushEntry(eq, x + (y - 1) * n, dsigxym);
    pushEntry(eq, x + y * n, dsigxy);
    pushEntry(eq, x + y * n + n * n, temp0.getsigcendiffx(x, y) / 2 / dx);
    pushEntry(eq, x + y * n + 2 * n * n, temp0.getsigcendiffy(x, y) / 2 / dx);
    if (x != n - 1)
        pushEntry(eq, x + 1 + y * n + n * n, temp0.sig(x, y) / 2 / dx);
    if (x != 0)
        pushEntry(eq, x - 1 + y * n + n * n, -temp0.sig(x, y) / 2 / dx);
    if (y != n - 1)
        pushEntry(eq, x + (y + 1) * n + 2 * n * n, temp0.sig(x, y) / 2 / dx);
    if (y != 0)
        pushEntry(eq, x + (y - 1) * n + 2 * n * n, -temp0.sig(x, y) / 2 / dx);
    f[eq] = 2 / dt * temp0.sig(x, y) * temp0.sig(x, y) + ((stt.getjjcendiffx(x, y).x + stt.getjjcendiffy(x, y).y) / 2 / dx + (stt.jj(x, y).x * stt.getsigcendiffx(x, y) + stt.jj(x, y).y * stt.getsigcendiffy(x, y)) / 2 / dx / stt.sig(x, y) - 2 * stt.sig(x, y) / dt) * temp0.sig(x, y)
        + (temp0.getjjcendiffx(x, y).x + temp0.getjjcendiffy(x, y).y) * temp0.sig(x, y) / 2 / dx + (temp0.jj(x, y).x * temp0.getsigcendiffx(x, y) + temp0.jj(x, y).y * temp0.getsigcendiffy(x, y)) / 2 / dx;
}

void pushjxeqentries(const int32_t x, const int32_t y)
{
    const int eq = x + y * n + n * n;
    double dsigxy = 2 / dt * temp0.jj(x, y).x + c0 * c0 * stt.getsigcendiffx(x, y) / 2 / dx + (stt.jj(x, y).x * stt.getjjcendiffx(x, y).x + stt.jj(x, y).y * stt.getjjcendiffy(x, y).x) / 2 / dx / stt.sig(x, y) - 2 * stt.jj(x, y).x / dt + c0 * c0 * temp0.getsigcendiffx(x, y) / 2 / dx;
    const double dsigxpy = c0 * c0 * temp0.sig(x, y) / 2 / dx;
    if (x == n - 1)
        dsigxy += dsigxpy;
    else
        pushEntry(eq, x + 1 + y * n, dsigxpy);
    const double dsigxmy = -c0 * c0 * temp0.sig(x, y) / 2 / dx;
    if (x == 0)
        dsigxy += dsigxmy;
    else
        pushEntry(eq, x - 1 + y * n, dsigxmy);
    pushEntry(eq, x + y * n, dsigxy);

    double djxxy = 2 / dt * temp0.sig(x, y) + temp0.getjjcendiffx(x, y).x / 2 / dx;
    pushEntry(eq, x + y * n + 2 * n * n, temp0.getjjcendiffy(x, y).x / 2 / dx);
    const double djxxyp = temp0.jj(x, y).y / 2 / dx;
    if (y == n - 1)
        djxxy += djxxyp;
    else
        pushEntry(eq, x + (y + 1) * n + n * n, djxxyp);
    const double djxxym = -temp0.jj(x, y).y / 2 / dx;
    if (y == 0)
        djxxy += djxxym;
    else
        pushEntry(eq, x + (y - 1) * n + n * n, djxxym);
    pushEntry(eq, x + y * n + n * n, djxxy);
    if (x != n - 1)
        pushEntry(eq, x + 1 + y * n + n * n, temp0.jj(x, y).x / 2 / dx);
    if (x != 0)
        pushEntry(eq, x - 1 + y * n + n * n, -temp0.jj(x, y).x / 2 / dx);
    f[eq] = 2 / dt * temp0.jj(x, y).x * temp0.sig(x, y) + (c0 * c0 * stt.getsigcendiffx(x, y) / 2 / dx + (stt.jj(x, y).x * stt.getjjcendiffx(x, y).x + stt.jj(x, y).y * stt.getjjcendiffy(x, y).x) / 2 / dx / stt.sig(x, y) - 2 * stt.jj(x, y).x / dt) * temp0.sig(x, y)
        + c0 * c0 * temp0.getsigcendiffx(x, y) * temp0.sig(x, y) / 2 / dx + (temp0.jj(x, y).x * temp0.getjjcendiffx(x, y).x + temp0.jj(x, y).y * temp0.getjjcendiffy(x, y).x) / 2 / dx;
}

void pushjyeqentries(const int32_t x, const int32_t y)
{
    const int eq = x + y * n + 2 * n * n;
    double dsigxy = 2 / dt * temp0.jj(x, y).y + c0 * c0 * stt.getsigcendiffy(x, y) / 2 / dx + (stt.jj(x, y).x * stt.getjjcendiffx(x, y).y + stt.jj(x, y).y * stt.getjjcendiffy(x, y).y) / 2 / dx / stt.sig(x, y) - 2 * stt.jj(x, y).y / dt + c0 * c0 * temp0.getsigcendiffy(x, y) / 2 / dx;
    const double dsigxyp = c0 * c0 * temp0.sig(x, y) / 2 / dx;
    if (y == n - 1)
        dsigxy += dsigxyp;
    else
        pushEntry(eq, x + (y + 1) * n, dsigxyp);
    const double dsigxym = -c0 * c0 * temp0.sig(x, y) / 2 / dx;
    if (y == 0)
        dsigxy += dsigxym;
    else
        pushEntry(eq, x + (y - 1) * n, dsigxym);
    pushEntry(eq, x + y * n, dsigxy);

    double djyxy = 2 / dt * temp0.sig(x, y) + temp0.getjjcendiffy(x, y).y / 2 /dx;
    pushEntry(eq, x + y * n + n * n, temp0.getjjcendiffx(x, y).y / 2 / dx);
    const double djyxpy = temp0.jj(x, y).x / 2 / dx;
    if (x == n - 1)
        djyxy += djyxpy;
    else
        pushEntry(eq, x + 1 + y * n + 2 * n * n, djyxpy);
    const double djyxmy = -temp0.jj(x, y).x / 2 / dx;
    if (x == 0)
        djyxy += djyxmy;
    else
        pushEntry(eq, x - 1 + y * n + 2 * n * n, djyxmy);
    pushEntry(eq, x + y * n + 2 * n * n, djyxy);
    if (y != n - 1)
        pushEntry(eq, x + (y + 1) * n + 2 * n * n, temp0.jj(x, y).y / 2 / dx);
    if (y != 0)
        pushEntry(eq, x + (y - 1) * n + 2 * n * n, temp0.jj(x, y).y / 2 / dx);
    f[eq] = 2 / dt * temp0.jj(x, y).y * temp0.sig(x, y) + (c0 * c0 * stt.getsigcendiffy(x, y) / 2 / dx + (stt.jj(x, y).x * stt.getjjcendiffx(x, y).y + stt.jj(x, y).y * stt.getjjcendiffy(x, y).y) / 2 / dx / stt.sig(x, y) - 2 * stt.jj(x, y).y / dt) * temp0.sig(x, y)
        + c0 * c0 * temp0.getsigcendiffy(x, y) * temp0.sig(x, y) / 2 / dx + (temp0.jj(x, y).x * temp0.getjjcendiffx(x, y).y + temp0.jj(x, y).y * temp0.getjjcendiffy(x, y).y) / 2 / dx;
}

bool newtonconverged;
bool newtondiverging;
double newtonlastfnorm = -1;
constexpr double newtontolearance = 1e-20;

void newton()
{
    auto t0 = chrono::steady_clock::now();
    nonzeros.clear();
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            pushsigeqentries(x, y);
            pushjxeqentries(x, y);
            pushjyeqentries(x, y);
        }
    }
    auto t01 = chrono::steady_clock::now();
    cout << "pushed in " << chrono::duration_cast<chrono::microseconds>(t01 - t0).count() << " us" << endl;
    jac.setFromTriplets(nonzeros.begin(), nonzeros.end());
    auto t1 = chrono::steady_clock::now();
    cout << "matrix set in " << chrono::duration_cast<chrono::microseconds>(t1 - t01).count() << " us" << endl;
    solver.compute(jac);
    auto t2 = chrono::steady_clock::now();
    cout << "computed in " << chrono::duration_cast<chrono::microseconds>(t2 - t1).count() << " us" << endl;
    const double fnorm = f.squaredNorm();
    cout << "f norm: " << fnorm << endl;
    if (fnorm <= newtontolearance)
        newtonconverged = true;
    if (newtonlastfnorm != -1 && fnorm > newtonlastfnorm)
        newtondiverging = true;
    newtonlastfnorm = fnorm;
    //checkf();
    jacsol = solver.solve(f);
    auto t3 = chrono::steady_clock::now();
    cout << "solved in " << chrono::duration_cast<chrono::microseconds>(t3 - t2).count() << " us" << endl;
    //cout << "deltax norm: " << jacsol.squaredNorm() << endl;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            temp0.sig(x, y) -= jacsol[x + y * n];
            temp0.jj(x, y).x -= jacsol[x + y * n + n * n];
            temp0.jj(x, y).y -= jacsol[x + y * n + 2 * n * n];
        }
    }
}

void iter()
{
    uint32_t needed = 1;
    uint32_t done = 0;
    dt = maxdt;
    while (done < needed)
    {
        temp1 = temp0;
        newtonconverged = false;
        newtondiverging = false;
        newtonlastfnorm = -1;
        uint32_t iters = 0;
        while (!newtonconverged)
        {
            newton();
            iters++;
            if (newtondiverging)
            {
                dt /= 2;
                needed *= 2;
                done *= 2;
                temp0 = temp1;
                break;
            }
        }
        if (newtondiverging)
        {
            cout << "diverging newton, refined time step" << endl;
        }
        else
        {
            cout << "newton converged after " << iters << " iterations" << endl;
            done++;
            stt = temp0;
        }
    }
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
    auto start = chrono::steady_clock::now();
    constexpr uint32_t mf = 1;
    CImg<unsigned char> img(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = (stt.sig(x, y) + .2f) * 127;
            img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = clamp(val);
            /*const int val0 = (jj1[x + y * n].x * 500 + .5) * 127;
            const int val1 = (jj1[x + y * n].y * 500 + .5) * 127;
            img(x, y, 0) = val0;
            img(x, y, 1) = val1;
            img(x, y, 2) = 0;*/
        }
    }
    CImg<unsigned char> imgjx(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = stt.jj(x, y).x * 32 * 127 + 127;
            imgjx(x, y, 0) = clamp(val);
        }
    }
    CImg<unsigned char> imgjy(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = stt.jj(x, y).y * 32 * 127 + 127;
            imgjy(x, y, 2) = clamp(val);
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
    imgjx.save(("jx" + name).c_str());
    imgjy.save(("jy" + name).c_str());
    auto end = chrono::steady_clock::now();
    cout << "save in " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " us" << endl;
}

void stats(const uint32_t iter)
{
    double total = 0;
    for (uint32_t i = 0; i < n * n; i++)
    {
        total += stt.sig.data[i];
    }
    cout << iter << ": " << total << endl;
}

int main()
{
    cout.precision(17);
    /*uint32_t threads = 8;
    omp_set_num_threads(threads);
    Eigen::setNbThreads(threads);*/
    init();
    for (uint32_t i = 0; ; i++)
    {
        save(to_string(i) + ".png");
        stats(i);
        iter();
    }
    return EXIT_SUCCESS;
}