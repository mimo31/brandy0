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
    return vec2d((x + .5) / double(n) - .5, (y + .5) / double(n) - .5);
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
            freepoints(x, y) = !(tocoor(x, y).len2() < midr * midr);
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
            if (freepoints(x, y))
            {
                point_list[next_ind] = Point(x, y);
                next_ind++;
            }
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
        //vdep(cor, -1, VarType::Uy).push_back(LinearVar(2, pointnums(0, cor) + freecount));
        //vdep(cor, -1, VarType::Uy).push_back(LinearVar(-1, pointnums(1, cor) + freecount));
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
    for (int32_t y = 1; y < n - 1; y++)
    {
        for (int32_t x = 1; x < n - 1; x++)
        {
            if (freepoints(x, y))
                continue;
            const int32_t movex = x < n / 2 ? -1 : 1;
            const int32_t movey = y < n / 2 ? -1 : 1;
            const bool mvxfree = freepoints(x + movex, y);
            const bool mvyfree = freepoints(x, y + movey);
            if (!mvxfree && !mvyfree)
                continue;
            const vec2d f = tocoor(-y, x);
            const uint32_t pcou = (mvxfree && mvyfree) ? 2 : 1;
            if (mvxfree)
            {
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.x / f.len2() / pcou, pointnums(x + movex, y)));
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.y / f.len2() / pcou, pointnums(x + movex, y) + freecount));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.x * f.y / f.len2() / pcou, pointnums(x + movex, y)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.y * f.y / f.len2() / pcou, pointnums(x + movex, y) + freecount));
                vdep(x, y, VarType::P).push_back(LinearVar(1 / double(pcou), pointnums(x + movex, y) + 2 * freecount));
            }
            if (mvyfree)
            {
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.x / f.len2() / pcou, pointnums(x, y + movey)));
                vdep(x, y, VarType::Ux).push_back(LinearVar(f.x * f.y / f.len2() / pcou, pointnums(x, y + movey) + freecount));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.x * f.y / f.len2() / pcou, pointnums(x, y + movey)));
                vdep(x, y, VarType::Uy).push_back(LinearVar(f.y * f.y / f.len2() / pcou, pointnums(x, y + movey) + freecount));
                vdep(x, y, VarType::P).push_back(LinearVar(1 / double(pcou), pointnums(x, y + movey) + 2 * freecount));
            }
        }
    }
}

void Simulator::init_linear_algebra()
{
    f = Eigen::VectorXd(freecount * 3);
    jacsol = Eigen::VectorXd(freecount * 3);
    jac = Eigen::SparseMatrix<double, Eigen::RowMajor>(freecount * 3, freecount * 3); 
}

std::vector<LinearVar>& Simulator::vdep(const int32_t x, const int32_t y, const VarType vtype)
{
    return vardependence[x + 1 + (n + 2) * (y + 1) + (n + 2) * (n + 2) * vtype];
}

void Simulator::push_entry(const int eq, const int var, const double val)
{
    if (eq < 0)
        cout << eq << endl;
    if (var < 0)
        cout << var << endl;
    nonzeros.push_back(Eigen::Triplet<double>(eq, var, val));
}

void Simulator::push_safes()
{
    for (int32_t y = 1; y < n - 1; y++)
    {
        for (int32_t x = 1; x < n - 1; x++)
        {
            if (!safepoints(x, y))
                continue;
            const uint32_t massconseq = pointnums(x, y) + 2 * freecount;
            push_entry(massconseq, pointnums(x + 1, y), 1 / (2 * dx));
            push_entry(massconseq, pointnums(x - 1, y), -1 / (2 * dx));
            push_entry(massconseq, pointnums(x, y + 1) + freecount, 1 / (2 * dx));
            push_entry(massconseq, pointnums(x, y - 1) + freecount, -1 / (2 * dx));
            f[massconseq] = (s1.u(x + 1, y).x - s1.u(x - 1, y).x + s1.u(x, y + 1).y - s1.u(x, y - 1).y) / (2 * dx);
            const uint32_t xtimeeq = pointnums(x, y);
            push_entry(xtimeeq, pointnums(x, y), 2 * rho / dt + (s1.u(x + 1, y).x - s1.u(x - 1, y).x) / (2 * dx) + 4 * mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x - 1, y), -s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x + 1, y), s1.u(x, y).x / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y - 1), -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y + 1), s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(xtimeeq, pointnums(x, y) + freecount, (s1.u(x, y + 1).x - s1.u(x, y - 1).x) / (2 * dx));
            push_entry(xtimeeq, pointnums(x - 1, y) + 2 * freecount, -1 / (2 * dx));
            push_entry(xtimeeq, pointnums(x + 1, y) + 2 * freecount, 1 / (2 * dx));
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
            push_entry(ytimeeq, pointnums(x, y - 1) + freecount, -s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x, y + 1) + freecount, s1.u(x, y).y / (2 * dx) - mu / (dx * dx));
            push_entry(ytimeeq, pointnums(x, y), (s1.u(x + 1, y).y - s1.u(x - 1, y).y) / (2 * dx));
            push_entry(ytimeeq, pointnums(x, y - 1) + 2 * freecount, -1 / (2 * dx));
            push_entry(ytimeeq, pointnums(x, y + 1) + 2 * freecount, 1 / (2 * dx));
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

double Simulator::var_value(const State& st, const uint32_t var)
{
    if (var == CONST_VAR)
        return 1;
    if (var < freecount)
        return st.u(point_list[var]).x;
    else if (var < freecount * 2)
        return st.u(point_list[var - freecount]).y;
    else
        return st.p(point_list[var - freecount * 2]);
}

double Simulator::dependent_var_value(const State& st, const int32_t x, const int32_t y, const VarType vtype)
{
    if (x >= 0 && x < n && y >= 0 && y < n && freepoints(x, y))
        return var_value(st, pointnums(x, y) + vtype * freecount);
    double val = 0;
    for (const LinearVar lv : vdep(x, y, vtype))
    {
        val += lv.mult * var_value(st, lv.var);
    }
    return val;
}

void Simulator::add_par_ders(PartialDers& ders, const double coeff, const int32_t x, const int32_t y, const VarType vtype)
{
    if (x >= 0 && x < n && y >= 0 && y < n && freepoints(x, y))
    {
        ders.add(coeff, pointnums(x, y) + vtype * freecount);
    }
    else
    {
        for (const LinearVar lv : vdep(x, y, vtype))
        {
            if (lv.var != CONST_VAR)
                ders.add(coeff * lv.mult, lv.var);
        }
    }
}

void Simulator::push_equation(const int eq, const PartialDers& ders)
{
    for (const LinearVar lv : ders.ders)
    {
        push_entry(eq, lv.var, lv.mult);
    }
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
            const double uxxpytp = dependent_var_value(s1, x + 1, y, VarType::Ux);
            const double uxxmytp = dependent_var_value(s1, x - 1, y, VarType::Ux);
            const double uyxyptp = dependent_var_value(s1, x, y + 1, VarType::Uy);
            const double uyxymtp = dependent_var_value(s1, x, y - 1, VarType::Uy);
            f[massconseq] = (uxxpytp - uxxmytp + uyxyptp - uyxymtp) / (2 * dx);
            PartialDers massconsders;
            add_par_ders(massconsders, 1 / (2 * dx), x + 1, y, VarType::Ux);
            add_par_ders(massconsders, -1 / (2 * dx), x - 1, y, VarType::Ux);
            add_par_ders(massconsders, 1 / (2 * dx), x, y + 1, VarType::Uy);
            add_par_ders(massconsders, -1 / (2 * dx), x, y - 1, VarType::Uy);
            push_equation(massconseq, massconsders);
            const uint32_t xtimeeq = pointnums(x, y);
            const double uxxytp = s1.u(x, y).x;
            const double uxxyt = s0.u(x, y).x;
            const double uyxytp = s1.u(x, y).y;
            const double uxxyptp = dependent_var_value(s1, x, y + 1, VarType::Ux);
            const double uxxymtp = dependent_var_value(s1, x, y - 1, VarType::Ux);
            const double pxpytp = dependent_var_value(s1, x + 1, y, VarType::P);
            const double pxmytp = dependent_var_value(s1, x - 1, y, VarType::P);
            const double uxxpyt = dependent_var_value(s0, x + 1, y, VarType::Ux);
            const double uxxmyt = dependent_var_value(s0, x - 1, y, VarType::Ux);
            const double uyxyt = s0.u(x, y).y;
            const double uxxypt = dependent_var_value(s0, x, y + 1, VarType::Ux);
            const double uxxymt = dependent_var_value(s0, x, y - 1, VarType::Ux);
            const double pxpyt = dependent_var_value(s0, x + 1, y, VarType::P);
            const double pxmyt = dependent_var_value(s0, x - 1, y, VarType::P);
            f[xtimeeq] = 2 * rho * (uxxytp - uxxyt) / dt // d/dt ux
                + uxxytp * (uxxpytp - uxxmytp) / (2 * dx) + uyxytp * (uxxyptp - uxxymtp) / (2 * dx) // u dot grad ux (t + 1)
                + (pxpytp - pxmytp) / (2 * dx) // (grad p).x (t + 1)
                - mu * (uxxpytp + uxxmytp + uxxyptp + uxxymtp - 4 * uxxytp) / (dx * dx) // lap ux (t + 1)
                + uxxyt * (uxxpyt - uxxmyt) / (2 * dx) + uyxyt * (uxxypt - uxxymt) / (2 * dx) // u dot grad ux (t)
                + (pxpyt - pxmyt) / (2 * dx) // (grad p).x (t)
                - mu * (uxxpyt + uxxmyt + uxxypt + uxxymt - 4 * uxxyt) / (dx * dx); // lap ux (t)
            PartialDers xtimeders;
            add_par_ders(xtimeders, 2 * rho / dt, x, y, VarType::Ux);
            add_par_ders(xtimeders, (uxxpytp - uxxmytp) / (2 * dx), x, y, VarType::Ux);
            add_par_ders(xtimeders, uxxytp / (2 * dx), x + 1, y, VarType::Ux);
            add_par_ders(xtimeders, -uxxytp / (2 * dx), x - 1, y, VarType::Ux);
            add_par_ders(xtimeders, (uxxyptp - uxxymtp) / (2 * dx), x, y, VarType::Uy);
            add_par_ders(xtimeders, uyxytp / (2 * dx), x, y + 1, VarType::Ux);
            add_par_ders(xtimeders, -uyxytp / (2 * dx), x, y - 1, VarType::Ux);
            add_par_ders(xtimeders, 1 / (2 * dx), x + 1, y, VarType::P);
            add_par_ders(xtimeders, -1 / (2 * dx), x - 1, y, VarType::P);
            add_par_ders(xtimeders, -mu / (dx * dx), x + 1, y, VarType::Ux);
            add_par_ders(xtimeders, -mu / (dx * dx), x - 1, y, VarType::Ux);
            add_par_ders(xtimeders, -mu / (dx * dx), x, y + 1, VarType::Ux);
            add_par_ders(xtimeders, -mu / (dx * dx), x, y - 1, VarType::Ux);
            add_par_ders(xtimeders, 4 * mu / (dx * dx), x, y, VarType::Ux);
            push_equation(xtimeeq, xtimeders);
            const uint32_t ytimeeq = pointnums(x, y) + freecount;
            const double uyxpytp = dependent_var_value(s1, x + 1, y, VarType::Uy);
            const double uyxmytp = dependent_var_value(s1, x - 1, y, VarType::Uy);
            const double pxyptp = dependent_var_value(s1, x, y + 1, VarType::P);
            const double pxymtp = dependent_var_value(s1, x, y - 1, VarType::P);
            const double uyxpyt = dependent_var_value(s0, x + 1, y, VarType::Uy);
            const double uyxmyt = dependent_var_value(s0, x - 1, y, VarType::Uy);
            const double uyxypt = dependent_var_value(s0, x, y + 1, VarType::Uy);
            const double uyxymt = dependent_var_value(s0, x, y - 1, VarType::Uy);
            const double pxypt = dependent_var_value(s0, x, y + 1, VarType::P);
            const double pxymt = dependent_var_value(s0, x, y - 1, VarType::P);
            f[ytimeeq] = 2 * rho * (uyxytp - uyxyt) / dt // d/dt uy
                + uxxytp * (uyxpytp - uyxmytp) / (2 * dx) + uyxytp * (uyxyptp - uyxymtp) / (2 * dx) // u dot grad uy (t + 1)
                + (pxyptp - pxymtp) / (2 * dx) // (grad p).y (t + 1)
                - mu * (uyxpytp + uyxmytp + uyxyptp + uyxymtp - 4 * uyxytp) / (dx * dx) // lap uy (t + 1)
                + uxxyt * (uyxpyt - uyxmyt) / (2 * dx) + uyxyt * (uyxypt - uyxymt) / (2 * dx) // u dot grad uy (t)
                + (pxypt - pxymt) / (2 * dx) // (grad p).y (t)
                - mu * (uyxpyt + uyxmyt + uyxypt + uyxymt - 4 * uyxyt) / (dx * dx); // lap uy (t)
            PartialDers ytimeders;
            add_par_ders(ytimeders, 2 * rho / dt, x, y, VarType::Uy);
            add_par_ders(ytimeders, (uyxpytp - uyxmytp) / (2 * dx), x, y, VarType::Ux);
            add_par_ders(ytimeders, uxxytp / (2 * dx), x + 1, y, VarType::Uy);
            add_par_ders(ytimeders, -uxxytp / (2 * dx), x - 1, y, VarType::Uy);
            add_par_ders(ytimeders, (uyxyptp - uyxymtp) / (2 * dx), x, y, VarType::Uy);
            add_par_ders(ytimeders, uyxytp / (2 * dx), x, y + 1, VarType::Uy);
            add_par_ders(ytimeders, -uyxytp / (2 * dx), x, y - 1, VarType::Uy);
            add_par_ders(ytimeders, 1 / (2 * dx), x, y + 1, VarType::P);
            add_par_ders(ytimeders, -1 / (2 * dx), x, y - 1, VarType::P);
            add_par_ders(ytimeders, -mu / (dx * dx), x + 1, y, VarType::Uy);
            add_par_ders(ytimeders, -mu / (dx * dx), x - 1, y, VarType::Uy);
            add_par_ders(ytimeders, -mu / (dx * dx), x, y + 1, VarType::Uy);
            add_par_ders(ytimeders, -mu / (dx * dx), x, y - 1, VarType::Uy);
            add_par_ders(ytimeders, 4 * mu / (dx * dx), x, y, VarType::Uy);
            push_equation(ytimeeq, ytimeders);
        }
    }
}

constexpr uint32_t itersperstep = 1;

void Simulator::iter()
{
    /*for (uint32_t j = 0; j < freecount; j++)
    {
    double delta = 0.0001;
    Eigen::VectorXd col(freecount * 3);
    for (uint32_t i = 0; i < freecount * 3; i++)
    {
        col[i] = 0;
    }
    col[j + 2 * freecount] = delta;
    nonzeros.clear();
    push_safes();
    push_unsafes();
    double* prevf = new double[freecount * 3];
    for (uint32_t i = 0; i < freecount * 3; i++)
    {
        prevf[i] = f[i];
    }
    jac.setFromTriplets(nonzeros.begin(), nonzeros.end());
    Eigen::VectorXd prod = jac * col;
    for (uint32_t i = 0; i < freecount * 3; i++)
    {
        if (prod[i] != 0)
            cout << i << ": " << prod[i] << endl;
    }
    s1.p(point_list[j]) += delta;
    nonzeros.clear();
    push_safes();
    push_unsafes();
    for (uint32_t i = 0; i < freecount * 3; i++)
    {
        if (abs(f[i] - prevf[i] - prod[i]) > 0.0001)
        {
            cout << j << ", " << i << ": " << f[i] << " from " << prevf[i] << " (delta = " << f[i] - prevf[i] << ")" << endl;
            cout << "prod = " << prod[i] << endl;
        }
    }
    s1.p(point_list[j]) = 1;
    }
    return;*/
    // let state 1 be the current best approximation of the next state (assumed)
    //double* original = new double[3 * freecount];
    for (uint32_t i = 0; i < 2; i++)
    {
        nonzeros.clear();
        push_safes();
        push_unsafes();
        /*for (uint32_t j = 0; j < 3 * freecount; j += 16)
        {
            if (f[j] != 0)
                cout << j << ": " << f[j] << " (" << original[j] << ")" << endl;
            original[j] = f[j];
        }*/
        jac.setFromTriplets(nonzeros.begin(), nonzeros.end());
        solver.compute(jac);
        const double fnorm = f.squaredNorm();
        cout << fnorm << endl;
        jacsol = solver.solve(f);
        //cout << "solving" << endl;
        for (uint32_t y = 0; y < n; y++)
        {
            for (uint32_t x = 0; x < n; x++)
            {
                if (!freepoints(x, y))
                    continue;
                s1.u(x, y).x -= jacsol[pointnums(x, y)];
                s1.u(x, y).y -= jacsol[pointnums(x, y) + freecount];
                s1.p(x, y) -= jacsol[pointnums(x, y) + 2 * freecount];
            }
        }
    }
    s0 = s1;
}

}