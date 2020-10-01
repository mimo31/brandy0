#include <iostream>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

constexpr int32_t n = 1024;
constexpr double dx = 1 / double(n) / 128;
constexpr double dt = .001;

double* sig;
double* sigdt;
double* lap;

void init()
{
    sig = new double[n * n];
    sigdt = new double[n * n];
    lap = new double[n * n];
    fill_n(sig, n * n, 0);
    fill_n(sigdt, n * n, 0);
    //fill_n(lap, n * n, 0);
    /*for (int32_t i = 0; i < n; i++)
    {
        for (int32_t j = 0; j < n; j++)
        {
            const double cendist = max(abs(i - n / 2), abs(j - n / 2));
            sig[i + j * n] = max(0.0, min((n / 4 - cendist) / 16.0, 1.0));
        }
    }*/
    for (int32_t i = n / 4; i < 3 * n / 4; i++)
    {
        for (int32_t j = i; j < 3 * n / 4; j++)
        {
            sig[i + j * n] = 1;
        }
    }
}

double sg(const int32_t x, const int32_t y)
{
    return sig[min(max(x, 0), n - 1) + n * min(max(y, 0), n - 1)];
}

void calclap()
{
    for (int32_t i = 0; i < n; i++)
    {
        for (int32_t j = 0; j < n; j++)
        {
            lap[i + j * n] = (sg(i + 1, j) + sg(i - 1, j) + sg(i, j + 1) + sg(i, j - 1) - 4 * sg(i, j)) / dx / dx;
        }
    }
}

void applylap()
{
    constexpr double rtm = .00001;
    for (int32_t i = 0; i < n; i++)
    {
        for (int32_t j = 0; j < n; j++)
        {
            sig[i + j * n] = sig[i + j * n] + dt * sigdt[i + j * n] + dt * dt / 2 * rtm * lap[i + j * n];
            sigdt[i + j * n] = sigdt[i + j * n] + dt * rtm * lap[i + j * n];
        }
    }
}

void save(const string name)
{
    CImg<unsigned char> img(n, n, 1, 3, 0);
    for (uint32_t i = 0; i < n; i++)
    {
        for (uint32_t j = 0; j < n; j++)
        {
            img(i, j, 0) = img(i, j, 1) = img(i, j, 2) = int(sig[i + j * n] * 127);
        }
    }
    img.save(name.c_str());
}

int main()
{
    init();
    for (uint32_t i = 0; i < 30; i++)
    {
        save(to_string(i) + ".png");
        for (uint32_t j = 0; j < 100; j++)
        {
            calclap();
            applylap();
        }
    }
    return EXIT_SUCCESS;
}