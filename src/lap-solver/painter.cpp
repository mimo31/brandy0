/**
 * painter.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#include "painter.h"

#include <string>

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
std::string save_path = "../../output/";

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
            const int val = 127 + s.s0.p(x, y) * 160;
            pimg(x, y, 0) = pimg(x, y, 1) = pimg(x, y, 2) = clamp(val);
        }
    }
    pimg.save((save_path + "p" + filename).c_str());
    constexpr uint32_t spac = 8;
    constexpr uint32_t iters = 8000;
    constexpr double chngc = .003;
    cimg_library::CImg<unsigned char> pelimg(n, n, 1, 3, 0);
    for (uint32_t y = spac / 2; y < n; y += spac)
    {
        for (uint32_t x = spac / 2; x < n; x += spac)
        {
            vec2d pos(x * dx, y * dx);
            for (uint32_t i = 0; i < iters; i++)
            {
                const int32_t corx = int32_t(pos.x / dx);
                const int32_t cory = int32_t(pos.y / dx);
                if (corx <= 0 || corx >= n - 1 || cory <= 0 || cory >= n - 1)
                {
                    //cout << "i = " << i << "; corx = " << corx << "; cory = " << cory << endl;
                    break;
                }
                setcol(pelimg, Col(255, 255, 255), corx, cory);
                const double difx = (s.s0.p(corx + 1, cory) - s.s0.p(corx - 1, cory)) / 2 / dx;
                const double dify = (s.s0.p(cory, cory + 1) - s.s0.p(corx, cory - 1)) / 2 / dx;
                pos.x += dify * chngc;
                pos.y += -difx * chngc;
            }
        }
    }
    pelimg.save((save_path + "pel" + filename).c_str());
    cimg_library::CImg<unsigned char> ufimg(n, n, 1, 3, 0);
    for (uint32_t y = spac / 2; y < n; y += spac)
    {
        for (uint32_t x = spac / 2; x < n; x += spac)
        {
            vec2d pos(x * dx, y * dx);
            for (uint32_t i = 0; i < iters; i++)
            {
                const int32_t corx = int32_t(pos.x / dx);
                const int32_t cory = int32_t(pos.y / dx);
                if (corx <= 0 || corx >= n - 1 || cory <= 0 || cory >= n - 1)
                {
                    //cout << "i = " << i << "; corx = " << corx << "; cory = " << cory << endl;
                    break;
                }
                setcol(ufimg, Col(255, 255, 255), corx, cory);
                pos.x += s.s0.u(corx, cory).x * chngc;
                pos.y += s.s0.u(corx, cory).y * chngc;
            }
        }
    }
    ufimg.save((save_path + "uf" + filename).c_str());
    cimg_library::CImg<unsigned char> uyimg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = 127 + s.s0.u(x, y).y * 100;
            uyimg(x, y, 0) = uyimg(x, y, 1) = uyimg(x, y, 2) = clamp(val);
        }
    }
    uyimg.save((save_path + "uy" + filename).c_str());
    cimg_library::CImg<unsigned char> uximg(n, n, 1, 3, 0);
    for (uint32_t x = 0; x < n; x++)
    {
        for (uint32_t y = 0; y < n; y++)
        {
            const int val = 127 + s.s0.u(x, y).x * 100;
            uximg(x, y, 0) = uximg(x, y, 1) = uximg(x, y, 2) = clamp(val);
        }
    }
    uximg.save((save_path + "ux" + filename).c_str());
}

}