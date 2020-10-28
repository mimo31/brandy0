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

    vec2d operator+(const vec2d &other) const
    {
        return vec2d(x + other.x, y + other.y);
    }

    vec2d operator-(const vec2d &other) const
    {
        return vec2d(x - other.x, y - other.y);
    }

    vec2d& operator*=(const double a)
    {
        x *= a;
        y *+ a;
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

constexpr int32_t n = 64;
constexpr double dx = 3 / 512.0;
constexpr double dt = .04;
constexpr double c0 = .003;

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

    Grid &operator=(const Grid& other)
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

    double getsigdt(const int32_t x, const int32_t y) const
    {
        return -(getjj(x + 1, y).x - getjj(x - 1, y).x + getjj(x, y + 1).y - getjj(x, y - 1).y) / 2 / dx - (getjj(x, y).x * (getsig(x + 1, y) - getsig(x - 1, y)) + getjj(x, y).y * (getsig(x, y + 1) - getsig(x, y - 1))) / 2 / dx / getsig(x, y);
    }

    vec2d getjjdt(const int32_t x, const int32_t y) const
    {
        return vec2d(-c0 * c0 * (getsig(x + 1, y) - getsig(x - 1, y)) / 2 / dx - (getjj(x, y).x * (getjj(x + 1, y).x - getjj(x - 1, y).x) + getjj(x, y).y * (getjj(x, y + 1).x - getjj(x, y - 1).x)) / 2 / dx / getsig(x, y),
            -c0 * c0 * (getsig(x, y + 1) - getsig(x, y - 1)) / 2 / dx - (getjj(x, y).x * (getjj(x + 1, y).y - getjj(x - 1, y).y) + getjj(x, y).y * (getjj(x, y + 1).y - getjj(x, y - 1).y)) / 2 / dx / getsig(x, y)
        );
    }

};

constexpr double alph = .1;
constexpr double c1 = .0001;
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

double newsig(const State& olds, const State& news, const int32_t x, const int32_t y)
{
    return olds.getsig(x, y) + dt / 2 * (olds.getsigdt(x, y) + news.getsigdt(x, y));
}

vec2d newjj(const State& olds, const State& news, const int32_t x, const int32_t y)
{
    return olds.getjj(x, y) + dt / 2 * (olds.getjjdt(x, y) + news.getjjdt(x, y));
}

double sigerror(const State& olds, const State& news)
{
    double total = 0;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            double diff = news.sig(x, y) - newsig(olds, news, x, y);
            total += diff * diff;
        }
    }
    return total;
}

double jjerror(const State& olds, const State& news)
{
    double total = 0;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            total += (news.jj(x, y) - newjj(olds, news, x, y)).len2();
        }
    }
    return total;
}

double sigerrorat(const State& olds, const State& news, const int32_t x, const int32_t y)
{
    double total = 0;
    if (x > 0)
    {
        const double diff = news.sig(x - 1, y) - newsig(olds, news, x - 1, y);
        total += diff * diff;
    }
    if (x < n - 1)
    {
        const double diff = news.sig(x + 1, y) - newsig(olds, news, x + 1, y);
        total += diff * diff;
    }
    if (y > 0)
    {
        const double diff = news.sig(x, y - 1) - newsig(olds, news, x, y - 1);
        total += diff * diff;
    }
    if (y < n - 1)
    {
        const double diff = news.sig(x, y + 1) - newsig(olds, news, x, y + 1);
        total += diff * diff;
    }
    const double diff = news.sig(x, y) - newsig(olds, news, x, y);
    total += diff * diff;
    return total;
}

double jjerrorat(const State& olds, const State& news, const int32_t x, const int32_t y)
{
    double total = 0;
    if (x > 0)
    {
        total += (news.jj(x - 1, y) - newjj(olds, news, x - 1, y)).len2();
    }
    if (x < n - 1)
    {
        total += (news.jj(x + 1, y) - newjj(olds, news, x + 1, y)).len2();
    }
    if (y > 0)
    {
        total += (news.jj(x, y - 1) - newjj(olds, news, x, y - 1)).len2();
    }
    if (y < n - 1)
    {
        total += (news.jj(x, y + 1) - newjj(olds, news, x, y + 1)).len2();
    }
    total += (news.jj(x, y) - newjj(olds, news, x, y)).len2();
    return total;
}

constexpr double dv = .0001;

void iter()
{
    double sigdelta = .0001;
    double jjdelta = .0001;
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t moves = 0;
        for (uint32_t y = 0; y < n; y++)
        {
            for (uint32_t x = 0; x < n; x++)
            {
                double sig0 = temp0.sig(x, y);
                vec2d jj0 = temp0.jj(x, y);
                const double err0 = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.sig(x, y) += sigdelta;
                const double errsigp = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.sig(x, y) -= 2 * sigdelta;
                const double errsigm = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.sig(x, y) = sig0;
                temp0.jj(x, y).x += jjdelta;
                const double errjxp = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.jj(x, y).x -= 2 * jjdelta;
                const double errjxm = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.jj(x, y).x = jj0.x;
                temp0.jj(x, y).y += jjdelta;
                const double errjyp = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.jj(x, y).y -= 2 * jjdelta;
                const double errjym = sigerrorat(stt, temp0, x, y) + jjerrorat(stt, temp0, x, y);
                temp0.jj(x, y).y = jj0.y;
                /*if (x == 300 && y == 400 || errsigm < err0 || errsigp < err0 || errjxm < err0 || errjxp < err0 || errjym < err0 || errjyp < err0)
                {
                    cout << "point:" << endl;
                    cout << "sig " << errsigm << " " << err0 << " " << errsigp << endl;
                    cout << "jx " << errjxm << " " << err0 << " " << errjxp << endl;
                    cout << "jy " << errjym << " " << err0 << " " << errjyp << endl;
                }*/
                if (errsigp < err0)
                {
                    //const double e0 = sigerror(stt, temp0) + jjerror(stt, temp0);
                    temp0.sig(x, y) += sigdelta;
                    moves++;
                    //const double e1 = sigerror(stt, temp0) + jjerror(stt, temp0);
                    //cout << e1 - e0 << endl;
                    //cout << errsigp - err0 << endl;
                    //cout << "good +" << endl;
                }
                if (errsigm < err0)
                {
                    temp0.sig(x, y) -= sigdelta;
                    moves++;
                    //cout << "good -" << endl;
                }
                if (errjxp < err0)
                {
                    temp0.jj(x, y).x += jjdelta;
                    moves++;
                }
                if (errjxm < err0)
                {
                    temp0.jj(x, y).x -= jjdelta;
                    moves++;
                }
                if (errjyp < err0)
                {
                    temp0.jj(x, y).y += jjdelta;
                    moves++;
                }
                if (errjym < err0)
                {
                    temp0.jj(x, y).y -= jjdelta;
                    moves++;
                }
                //temp1.sig(x, y) = temp0.sig(x, y) - alph * (err1 - err0);
                /*double newsig = (1 - alph) * temp0.sig(x, y) + alph * (stt.sig(x, y) + dt / 2 * (stt.getsigdt(x, y) + temp0.getsigdt(x, y)));
                vec2d newjj = (1 - alph) * temp0.jj(x, y) + alph * (stt.jj(x, y) + dt / 2 * (stt.getjjdt(x, y) + temp0.getjjdt(x, y)));
                double sigd = abs(temp1.sig(x, y) - newsig);
                double jjd = (temp1.jj(x, y) - newjj).len2();
                if (x == 0 && y == 0 || sigd > maxsigd)
                    maxsigd = sigd;
                if (x == 0 && y == 0 || jjd > maxjjd)
                    maxjjd = jjd;
                temp1.sig(x, y) = newsig;
                temp1.jj(x, y) = newjj;*/
            }
        }
        temp1 = temp0;
        cout << i << ": " << sigerror(stt, temp1) + jjerror(stt, temp1) << ", " << moves << endl;
        if (moves < 50)
        {
            cout << "decr step" << endl;
            sigdelta /= 2;
            jjdelta /= 2;
        }
        //temp0 = temp1;
    }
    stt = temp0;
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
            const int val = (stt.sig(x, y) + .2f) * 127;
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
        total += stt.sig.data[i];
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
    cout.precision(17);
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
        /*for (uint32_t j = 0; j < 100; j++)
        {*/
            iter();
        //}
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