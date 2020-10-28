#include <iostream>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>
#include <vector>
#include <fstream>
#include <iomanip>

#include <omp.h>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

constexpr int32_t n = 512;
constexpr int32_t steps = 16;
constexpr double dx = 1 / 512.0;
constexpr double dt = .1;
constexpr double c0 = .003;
constexpr double c1 = .0001;

double *sig0;
double *sig1;
double *nxt;

void init()
{
    sig0 = new double[n * n];
    sig1 = new double[n * n];
    nxt = new double[n * n];
    for (int32_t x = 0; x < n; x++)
    {
        for (int32_t y = 0; y < n; y++)
        {
            const double corx = x / double(n);
            const double cory = y / double(n);
            //sig0[x + n * y] = sig1[x + n * y] = ((corx - .5) * (corx - .5) + (cory - .5) * (cory - .5) < .2) ? .8 : 0;
            sig0[x + n * y] = sig1[x + n * y] = 1.5 * exp(-((corx - .8) * (corx - .8) + (cory - .8) * (cory - .8)) * 128);
        }
        //sig0[i] = sig1[i] = (cor >= .25 && cor < .75) ? 1 : 0;
        //sig[i] = (0.5 - abs(cor - 0.5)) / .5;
        //sig0[i] = sig1[i] = sin(M_PI * 2 * cor);
    }
}

double sg(const int32_t x, const int32_t y)
{
    return sig1[min(max(x, 0), n - 1) + n * min(max(y, 0), n - 1)];
}

void iter()
{
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const double c = c0;//(x < n / 2 && 7 * n / 16 <= x) ? c1 : c0;
            nxt[x + y * n] = c * c * dt * dt / (dx * dx) * (sg(x - 1, y) + sg(x + 1, y) + sg(x, y - 1) + sg(x, y + 1) - 4 * sg(x, y)) - sig0[x + y * n] + 2 * sig1[x + y * n];
        }
    }
    swap(nxt, sig1);
    swap(nxt, sig0);
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
            const int val = sig1[x + y * n] * 127;
            img(x, y, 0) = img(x, y, 1) = img(x, y, 2) = clamp(val);
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
        total += sig1[i];
    }
    /*double noise = 0;
    for (uint32_t i = 0; i < n - 1; i++)
    {
        noise += abs(sig1[i] - sig1[i + 1]);
    }*/
    cout << iter << ": " << total << /*", " << noise << */endl;
}

void pushEntry(vector<Eigen::Triplet<double>> &vec, const int eq, const int var, const double val)
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
}

int main()
{
    /*uint32_t threads = 6;
    omp_set_num_threads(threads);
    Eigen::setNbThreads(threads);*/
    init();
    ofstream ofs("sol");
    ofs << fixed;
    ofs << setprecision(2);
    for (uint32_t i = 0; i < 1500; i++)
    {
        save(to_string(i) + ".png");
        stats(i);
        for (uint32_t x = 0; x < n; x++)
        {
            ofs << sig1[x] << " ";
        }
        ofs << '\n';
        for (uint32_t j = 0; j < 30; j++)
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