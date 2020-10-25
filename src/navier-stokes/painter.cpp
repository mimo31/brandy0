/**
 * painter.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#include "painter.h"

#include "CImg.h"

namespace brandy0
{

struct Col
{
    unsigned char r, g, b;

    constexpr Col(const unsigned char r, const unsigned char g, const unsigned char b) : r(r), g(g), b(b)
    {
    }
};

constexpr Col sp0(255, 255, 0);
constexpr Col sp1(0, 255, 0);

void setcol(cimg_library::CImg<unsigned char>& im, const Col& c, const uint32_t x, const uint32_t y)
{
    im(x, y, 0) = c.r;
    im(x, y, 1) = c.g;
    im(x, y, 2) = c.b;
}

int clamp(const int val)
{
    return std::min(std::max(val, 0), 255);
}

void save(const Simulator& s, const std::string& filename)
{
    cimg_library::CImg<unsigned char> pimg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            if (s.freepoints(x, y))
            {
                const int val = 127 + (s.s0.p(x, y) - 1) * 127;
                pimg(x, y, 0) = pimg(x, y, 1) = pimg(x, y, 2) = clamp(val);
            }
            else
            {
                setcol(pimg, (x < n / 2) ^ (y < n / 2) ? sp0 : sp1, x, y);
            }
        }
    }
    pimg.save(("pn" + filename).c_str());
    cimg_library::CImg<unsigned char> uyimg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            if (s.freepoints(x, y))
            {
                const int val = 127 - s.s0.u(x, y).y * 1000;
                uyimg(x, y, 0) = uyimg(x, y, 1) = uyimg(x, y, 2) = clamp(val);
            }
            else
            {
                setcol(uyimg, (x < n / 2) ^ (y < n / 2) ? sp0 : sp1, x, y);
            }
        }
    }
    uyimg.save(("uyn" + filename).c_str());
    cimg_library::CImg<unsigned char> uximg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            if (s.freepoints(x, y))
            {
                const int val = 127 + s.s0.u(x, y).x * 1000;
                uximg(x, y, 0) = uximg(x, y, 1) = uximg(x, y, 2) = clamp(val);
            }
            else
            {
                setcol(uximg, (x < n / 2) ^ (y < n / 2) ? sp0 : sp1, x, y);
            }
        }
    }
    uximg.save(("uxn" + filename).c_str());
}

}