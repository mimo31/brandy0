/**
 * simulator.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#include "simulator.h"

#include "grid.h"
#include "vec2d.h"
#include "state.h"

namespace brandy0
{

constexpr double dt = .00608;

constexpr double u0 = 9.5;

double to_coor(const uint32_t cor)
{
    return cor / double(n - 1);
}

void Simulator::init_values()
{
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            s0.p(x, y) = 0;
            s0.u(x, y) = vec2d(0, 0);
        }
    }
}

void Simulator::enforce_p_boundary()
{
    for (uint32_t x = 0; x < n; x++)
    {
        s0.p(x, 0) = 0;
        s0.p(x, n - 1) = 0;//s0.p(x, n - 2);
    }
    for (uint32_t y = 0; y < n; y++)
    {
        s0.p(0, y) = s0.p(1, y);
        s0.p(n - 1, y) = s0.p(n - 2, y);
    }
}

void Simulator::enforce_u_boundary()
{
    for (uint32_t x = 0; x < n; x++)
    {
        s0.u(x, 0).x = u0;
        s0.u(x, 0).y = u0 / 4;
        s0.u(x, n - 1).x = u0;//s0.u(x, n - 2).x;
        s0.u(x, n - 1).y = u0 / 4;
    }
    for (uint32_t y = 0; y < n; y++)
    {
        s0.u(0, y).x = 0;
        s0.u(0, y).y = 0;//s0.u(1, y).y;
        s0.u(n - 1, y).x = 0;
        s0.u(n - 1, y).y = 0;//s0.u(n - 2, y).y;
    }
}

void Simulator::enforce_boundary()
{
    enforce_p_boundary();
    enforce_u_boundary();
}

constexpr uint32_t itersperstep = 1;
constexpr uint32_t possion_iterations = 16000;

void Simulator::iter()
{
    s1 = s0;
    double fl1 = 0;
    for (uint32_t y = 1; y < n - 1; y++)
    {
        for (uint32_t x = 1; x < n - 1; x++)
        {
            f(x, y) = -(s1.u(x + 1, y).x - s1.u(x - 1, y).x + s1.u(x, y + 1).y - s1.u(x, y - 1).y) / dt / (2 * dx)
                + (s1.u(x + 1, y).x - s1.u(x - 1, y).x) * (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / 4 / (dx * dx)
                + 2 * (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / 2 / dx * (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / 2 / dx
                + (s1.u(x, y + 1).y - s1.u(x, y - 1).y) * (s1.u(x, y + 1).y - s1.u(x, y - 1).y) / 4 / (dx * dx);
            fl1 += abs(f(x, y));
        }
    }
    cout << "f L1 norm = " << fl1 << endl;
    //for (uint32_t i = 0; i < possion_iterations; i++)
    //{
    uint32_t i = 0;
    while (1)
    {
        s1.p = s0.p;
        double dl1 = 0;
        for (uint32_t y = 1; y < n - 1; y++)
        {
            for (uint32_t x = 1; x < n - 1; x++)
            {
                s0.p(x, y) = (s1.p(x + 1, y) + s1.p(x - 1, y) + s1.p(x, y + 1) + s1.p(x, y - 1) + (dx * dx) * f(x, y)) / 4;
                dl1 += abs(s0.p(x, y) - s1.p(x, y));
            }
        }
        if (i % 1000 == 0)
            cout << "dp L1 norm = " << dl1 << endl;
        enforce_p_boundary();
        if (dl1 < .1)
            break;
        i++;
    }
    for (uint32_t y = 1; y < n - 1; y++)
    {
        for (uint32_t x = 1; x < n - 1; x++)
        {
            s0.u(x, y).x = s1.u(x, y).x
                + dt * (
                    -s1.u(x, y).x * (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / 2 / dx - s1.u(x, y).y * (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / 2 / dx
                    - (s1.p(x + 1, y) - s1.p(x - 1, y)) / 2 / dx
                    + nu * (s1.u(x + 1, y).x + s1.u(x - 1, y).x + s1.u(x, y + 1).x + s1.u(x, y - 1).x - 4 * s1.u(x, y).x) / (dx * dx)
                );
            s0.u(x, y).y = s1.u(x, y).y
                + dt * (
                    -s1.u(x, y).x * (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / 2 / dx - s1.u(x, y).y * (s1.u(x, y + 1).y - s1.u(x, y - 1).y) / 2 / dx
                    - (s1.p(x, y + 1) - s1.p(x, y - 1)) / 2 / dx
                    + nu * (s1.u(x + 1, y).y + s1.u(x - 1, y).y + s1.u(x, y + 1).y + s1.u(x, y - 1).y - 4 * s1.u(x, y).y) / (dx * dx)
                );
        }
    }
    enforce_boundary();
}

}