/**
 * statgen.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#include "statgen.h"

#include <iostream>

#include "global.h"
#include "simulator.h"

namespace brandy0
{

void print_stats(const Simulator& s, const uint32_t iteration)
{
    /*
    double psum = 0;
    vec2d usum(0, 0);
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            if (!s.freepoints(x, y))
                continue;
            psum += s.s1.p(x, y);
            usum += s.s1.u(x, y);
        }
    }*/
    double l1 = 0;
    double l2 = 0;
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {
            const double dp = abs(s.s0.p(x, y) - s.s1.p(x, y));
            l1 += dp;
            l2 += dp * dp;
        }
    }
    cout << iteration << ": l1 norm = " << l1 << "; l2 norm = " << sqrt(l2) << endl;// << psum << ", usum = (" << usum.x << ", " << usum.y << ")" << std::endl;
}

}