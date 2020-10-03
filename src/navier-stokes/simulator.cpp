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
        }
    }
}

void Simulator::init_free_safe_indicators()
{
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            freepoints(x, y) = tocoor(x, y).len2() < midr * midr;
            if (freepoints(x, y))
            {
                pointnums(x, y) = freecount;
                freecount++;
            }
        }
    }
    point_list = new Point[freecount];
    uint32_t next_ind = 0;
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            if (x == 0 || x == n - 1 || y == 0 || y == n - 1)
                safepoints(x, y) = false;
            else
                safepoints(x, y) = freepoints(x, y) && freepoints(x + 1, y) && freepoints(x - 1, y) && freepoints(x, y + 1) && freepoints(x, y - 1);
            point_list[next_ind] = Point(x, y);
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

void Simulator::init_linear_algebra()
{
    f = Eigen::VectorXd(freecount);
    jacsol = Eigen::VectorXd(freecount);
    jac = Eigen::SparseMatrix<double, Eigen::RowMajor>(freecount, freecount); 
}

std::vector<LinearVar>& Simulator::vdep(const int32_t x, const int32_t y, const VarType vtype)
{
    return vardependence[x + 1 + (n + 2) * (y + 1) + (n + 2) * (n + 2) * vtype];
}

void Simulator::push_entry(const int eq, const int var, const double val)
{
    nonzeros.push_back(Eigen::Triplet<double>(eq, var, val));
}

void Simulator::push_safes()
{
    for (int32_t y = 1; y < n - 1; y++)
    {
        for (int32_t x = 1; x < n - 1; x++)
        {
            if (!safepoints(x, y))
                x = n - x;
            const uint32_t massconseq = pointnums(x, y) + 2 * freecount;
            push_entry(massconseq, pointnums(x + 1, y), 1);
            push_entry(massconseq, pointnums(x - 1, y), -1);
            push_entry(massconseq, pointnums(x, y + 1) + freecount, 1);
            push_entry(massconseq, pointnums(x, y - 1) + freecount, -1);
            f[massconseq] = s1.u(x + 1, y).x - s1.u(x - 1, y).x + s1.u(x, y + 1).y - s1.u(x, y - 1).y;
            const uint32_t xtimeeq = pointnums(x, y);
            push_entry(xtimeeq, pointnums(x, y), 2 * rho / dt + (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / (2 * dx) + 4 * mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x - 1, y), -s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x + 1, y), s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y - 1), -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y + 1), s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y) + freecount, (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / (2 * dx));
            f[xtimeeq] = 2 * rho * (s1.u(x, y).x - s0.u(x, y).x) / dt // time derivative
                + s1.u(x, y).x * (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / (2 * dx) + s1.u(x, y).y * (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / (2 * dx) // t + 1: u dot grad u
                + (s1.p(x + 1, y) - s1.p(x - 1, y)) / (2 * dx) // t + 1: grad p
                - mu * (s1.u(x + 1, y).x + s1.u(x - 1, y).x + s1.u(x, y + 1).x + s1.u(x, y - 1).x - 4 * s1.u(x, y).x) / (dx * dx) // t + 1: mu lap u
                + s0.u(x, y).x * (s0.u(x + 1, y).x - s0.u(x - 1, y).x) / (2 * dx) + s0.u(x, y).y * (s0.u(x, y + 1).x - s0.u(x, y - 1).x) / (2 * dx) // t: u dot grad u
                + (s0.p(x + 1, y) - s0.p(x - 1, y)) / (2 * dx) // t: grad p
                - mu * (s0.u(x + 1, y).x + s0.u(x - 1, y).x + s0.u(x, y + 1).x + s0.u(x, y - 1).x - 4 * s0.u(x, y).x) / (dx * dx); // t: mu lap u
            const uint32_t ytimeeq = pointnums(x, y) + freecount;
            push_entry(ytimeeq, pointnums(x, y) + freecount, 2 * rho / dt + (s1.u(x, y + 1).y - s1.u(x, y - 1).y) / (2 * dx) + 4 * mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x - 1, y) + freecount, -s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x + 1, y) + freecount, s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x, y - 1), -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x, y + 1), s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x, y), (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / (2 * dx));
            f[ytimeeq] = 2 * rho * (s1.u(x, y).y - s0.u(x, y).y) / dt // time derivative
                + s1.u(x, y).x * (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / (2 * dx) + s1.u(x, y).y * (s1.u(x, y + 1).y - s1.u(x, y - 1).y) / (2 * dx) // t + 1: u dot grad u
                + (s1.p(x, y + 1) - s1.p(x, y - 1)) / (2 * dx) // t + 1: grad p
                - mu * (s1.u(x + 1, y).y + s1.u(x - 1, y).y + s1.u(x, y + 1).y + s1.u(x, y - 1).y - 4 * s1.u(x, y).y) / (dx * dx) // t + 1: mu lap u
                + s0.u(x, y).x * (s0.u(x + 1, y).y - s0.u(x - 1, y).y) / (2 * dx) + s0.u(x, y).y * (s0.u(x, y + 1).y - s0.u(x, y - 1).y) / (2 * dx) // t: u dot grad u
                + (s0.p(x, y + 1) - s0.p(x, y - 1)) / (2 * dx) // t: grad p
                - mu * (s0.u(x + 1, y).y + s0.u(x - 1, y).y + s0.u(x, y + 1).y + s0.u(x, y - 1).y - 4 * s0.u(x, y).y) / (dx * dx); // t: mu lap u
        }
    }
}

struct PartialDers
{
    std::vector<LinearVar> ders;

    void add(const LinearVar d)
    {
        for (uint32_t i = 0; i < ders.size(); i++)
        {
            if (ders[i].var == d.var)
            {
                ders[i].mult += d.mult;
                return;
            }
        }
        ders.push_back(d);
    }

    void add(const double der, const uint32_t var)
    {
        add(LinearVar(der, var));
    }
};

double Simulator::var_value(const uint32_t var)
{
    if (var == CONST_VAR)
        return 1;
    if (var < freecount)
        return s1.u(point_list[var]).x;
    else if (var < freecount * 2)
        return s1.u(point_list[var - freecount]).y;
    else
        return s1.p(point_list[var - freecount * 2]);
}

void Simulator::push_unsafes()
{
    for (int32_t y = 0; y < n; y++)
    {
        for (int32_t x = 0; x < n; x++)
        {
            if (safepoints(x, y) || !freepoints(x, y))
                continue;
            const uint32_t massconseq = pointnums(x, y) + 2 * freecount;
            PartialDers massconsders;
            double uxxpy = 0;
            for (const LinearVar lv : vdep(x + 1, y, VarType::Ux))
            {
                massconsders.add(lv.var, lv.mult);
                uxxpy += lv.mult * 
            }
        }
    }
}

void Simulator::iter()
{
    // let state 1 be the current best approximation of the next state (assumed)
    for (uint32_t i = 0; i < itersperstep; i++)
    {

    }
    // iterate a few times (improve the current best approximation):
    //// show
    for (uint32_t y = 0; y < n; y++)
    {
        for (uint32_t x = 0; x < n; x++)
        {

        }
    }
    swap(sig1, nextsig);
    swap(sig0, nextsig);
    swap(jj1, nextjj);
    swap(jj0, nextjj);
    swap(t1, nextt);
    swap(t0, nextt);
}

}