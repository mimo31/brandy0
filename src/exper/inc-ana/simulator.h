/**
 * simulator.cpp
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/2
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/IterativeLinearSolvers>

#include "grid.h"
#include "vec2d.h"
#include "state.h"
#include "linearvar.h"
#include "vartype.h"
#include "point.h"
#include "partialders.h"

namespace brandy0
{

class Simulator
{

private:

    double dt;

    double u0;

    bool collapsed = false;

    Grid<double> f;

    double var_value(const State& st, const uint32_t var);
    double dependent_var_value(const State& st, const int32_t x, const int32_t y, const VarType vtype);
    void add_par_ders(PartialDers& ders, const double coeff, const int32_t x, const int32_t y, const VarType vtype);
    void push_entry(const int eq, const int var, const double val);
    void push_equation(const int eq, const PartialDers& ders);
    void push_safes();
    void push_unsafes();
    //std::vector<LinearVar>& vdep(const int32_t x, const int32_t y, const VarType vtype);
    
    void init_add_var(const uint32_t eq, const double coeff, const int32_t x, const int32_t y, const VarType vtype, Eigen::VectorXd& b, std::vector<Eigen::Triplet<double>>& nz);
    void init_values();
    void init_free_safe_indicators();
    void init_var_dependencies();
    void init_linear_algebra();

    void enforce_p_boundary();
    void enforce_u_boundary();
    void enforce_boundary();

    std::vector<LinearVar>& vdep(const int32_t x, const int32_t y, const VarType vtype) const;

public:

    void init()
    {
        init_values();
        enforce_boundary();
    }
    void iter();
    State s0, s1;
    Grid<bool> freepoints;
    bool has_collapsed();
    double step_change();
    double max_abs_u();

    Simulator(const double u0, const double dt) : u0(u0), dt(dt)
    {
    }

};

}

#endif // SIMULATOR_H