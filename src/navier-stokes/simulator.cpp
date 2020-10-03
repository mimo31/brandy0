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

inline vec2d tocoor(const int32_t x, const int32_t y)
{
    return vec2d(x / double(n) - .5, y / double(n) - .5);
}

void Simulator::init_values()
{
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            s0.u(x, y) = s1.u(x, y) = uenv;
            s0.p(x, y) = s1.p(x, y) = penv;
            freepoints(x, y) = tocoor(x, y).len2() < midr * midr;
            if (freepoints(x, y))
            {
                pointnums(x, y) = freecount;
                freecount++;
            }
        }
    }
}

void Simulator::init_free_safe_indicators()
{
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
}

uint32_t var_id(const uint32_t x, const uint32_t y, const VarType vtype)
{
    return x + n * y + n * n * vtype;
}

void Simulator::init_var_dependencies()
{
    vardependence = new std::vector<LinearVar>[(n + 2) * (n + 2) * 3];
    for (int32_t cor = 0; cor < n; cor++)
    {
        vdep(cor, -1, VarType::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(cor, n, VarType::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(cor, -1, VarType::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(cor, n, VarType::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(cor, -1, VarType::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(cor, n, VarType::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(-1, cor, VarType::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(n, cor, VarType::Ux).push_back(LinearVar(uenv.x, CONST_VAR));
        vdep(-1, cor, VarType::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(n, cor, VarType::Uy).push_back(LinearVar(uenv.y, CONST_VAR));
        vdep(-1, cor, VarType::P).push_back(LinearVar(penv, CONST_VAR));
        vdep(n, cor, VarType::P).push_back(LinearVar(penv, CONST_VAR));
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
            const vec2d f = tocoor(x, y);
            const uint32_t pcou = mvxfree && mvyfree ? 2 : 1;
            if (mvxfree)
            {
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.x / f.len2() / pcou, var_id(x + movex, y, VarType::Ux)));
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.y / f.len2() / pcou, var_id(x + movex, y, VarType::Uy)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.x * f.y / f.len2() / pcou, var_id(x + movex, y, VarType::Ux)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.y * f.y / f.len2() / pcou, var_id(x + movex, y, VarType::Uy)));
                vdep(x, y, VarType::P).push_back(LinearVar(1 / double(pcou), var_id(x + movex, y, VarType::P)));
            }
            if (mvyfree)
            {
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.x / f.len2() / pcou, var_id(x, y + movey, VarType::Ux)));
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.y / f.len2() / pcou, var_id(x, y + movey, VarType::Uy)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.x * f.y / f.len2() / pcou, var_id(x, y + movey, VarType::Ux)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.y * f.y / f.len2() / pcou, var_id(x, y + movey, VarType::Uy)));
                vdep(x, y, VarType::P).push_back(LinearVar(1 / double(pcou), var_id(x + movex, y, VarType::P)));
            }
        }
    }
}

std::vector<LinearVar>& Simulator::vdep(const int32_t x, const int32_t y, const VarType vtype)
{
    return vardependence[x + 1 + (n + 2) * (y + 1) + (n + 2) * (n + 2) * vtype];
}

}