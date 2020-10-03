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

constexpr double dx = 1 / 256.0; // space step
constexpr double dt = .00015;     // time step
constexpr double rho = 1;
constexpr double mu = 1;

class Simulator
{

private:

    Grid<bool> safepoints;
    Grid<uint32_t> pointnums;
    Point* point_list;
    uint32_t freecount = 0;
    std::vector<LinearVar>* vardependence;

    std::vector<Eigen::Triplet<double>> nonzeros;
    Eigen::VectorXd f;
    Eigen::VectorXd jacsol;
    Eigen::SparseMatrix<double, Eigen::RowMajor> jac;
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double, Eigen::RowMajor>> solver;
    uint32_t nexteq;

    double var_value(const State& st, const uint32_t var);
    double Simulator::dependent_var_value(const State& st, const int32_t x, const int32_t y, const VarType vtype);
    void Simulator::add_par_ders(PartialDers& ders, const double coeff, const int32_t x, const int32_t y, const VarType vtype);
    void push_entry(const int eq, const int var, const double val);
    void push_equation(const int eq, const PartialDers& ders);
    void push_safes();
    void push_unsafes();
    std::vector<LinearVar>& Simulator::vdep(const int32_t x, const int32_t y, const VarType vtype);
    
    void init_values();
    void init_free_safe_indicators();
    void init_var_dependencies();
    void init_linear_algebra();

    std::vector<LinearVar>& vdep(const int32_t x, const int32_t y, const VarType vtype) const;

public:

    void init()
    {
        init_values();
        init_free_safe_indicators();
        init_var_dependencies();
        init_linear_algebra();
    }
    void iter();
    State s0, s1;
    Grid<bool> freepoints;

};

}

#endif // SIMULATOR_H