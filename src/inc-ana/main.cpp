/**
 * main.cpp
 * 
 * Simulates fluid flow around an object by numerically solving the Navier-Stokes
 * equations using the Crank-Nicolson method
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/1
 */
#include "simulator.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>

#include "painter.h"
#include "statgen.h"

namespace brandy0
{

enum SimResult
{
    CONVERGED = 0, OSCILATING = 1, DIVERGED = 2, COLLAPSED = 3
};

struct SimEntry
{
    vec2d loc;
    SimResult res;

    SimEntry(const vec2d loc, const SimResult res) : loc(loc), res(res)
    {
    }
};

constexpr double MAX_U = 23;
constexpr double MAX_DT = .013;

double to_u(const double x)
{
    return MAX_U * x;
}

double to_dt(const double y)
{
    return MAX_DT * y;
}

double to_x(const double u)
{
    return u / MAX_U;
}

double to_y(const double dt)
{
    return dt / MAX_DT;
}

std::vector<SimEntry> entries;

SimResult simulate_point(const vec2d p)
{
    constexpr uint32_t STEPS = 50000;
    constexpr double CONV_LIM = 1e-9;
    constexpr double DIV_LIM = 1e6;

    Simulator sim(to_u(p.x), to_dt(p.y));
    sim.init();
    bool div = false;
    for (uint32_t i = 0; i < STEPS; i++)
    {
        sim.iter();
        if (sim.has_collapsed())
            return SimResult::COLLAPSED;
        if (!div && sim.step_change() < CONV_LIM)
            return SimResult::CONVERGED;
        if (!div && sim.max_abs_u() > DIV_LIM)
        {
            //cout << "div" << endl;
            div = true;
        }
        /*if (i % 100 == 0)
        {
            cout << sim.step_change() << " " << sim.max_abs_u() << endl;
        }*/
    }
    if (div)
        return SimResult::DIVERGED;
    return SimResult::OSCILATING;
}

std::uniform_real_distribution<double> unif(0, 1);
std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());

vec2d random_point()
{
    return vec2d(unif(re), unif(re));
}

double dist(const vec2d a, const vec2d b)
{
    return std::max(abs(a.x - b.x), abs(a.y - b.y));
}

double assign_score(const vec2d p)
{
    constexpr double BP = .9;
    constexpr uint32_t ST = 4;
    double* mns = new double[ST];
    for (uint32_t i = 0; i < ST; i++)
    {
        mns[i] = std::numeric_limits<double>::max();
    }
    for (const SimEntry& se : entries)
    {
        const double d = dist(se.loc, p);
        if (mns[se.res] == -1 || mns[se.res] > d)
        {
            mns[se.res] = d;
        }
    }
    std::sort(mns, mns + 4);
    return BP / mns[1] - 1 / mns[0];
}

vec2d next_loc()
{
    constexpr uint32_t N = 2048;
    vec2d best = random_point();
    double bestscore = assign_score(best);
    for (uint32_t i = 1; i < N; i++)
    {
        const vec2d p = random_point();
        const double sc = assign_score(p);
        if (sc > bestscore)
        {
            bestscore = sc;
            best = p;
        }
    }
    return best;
}

void add_entry()
{
    const vec2d loc = next_loc();
    cout << "sim u = " << to_u(loc.x) << ", dt = " << to_dt(loc.y);
    cout.flush();
    const SimResult rs = simulate_point(loc);
    entries.push_back(SimEntry(loc, rs));
    cout << " yld " << rs << endl;
}

const std::string SAVE_FILE = "entries.dat";

void save_entries()
{
    std::ofstream ofs;
    ofs.open(SAVE_FILE);
    ofs << entries.size() << endl;
    for (const SimEntry& se : entries)
    {
        ofs << to_u(se.loc.x) << " " << to_dt(se.loc.y) << " " << se.res << endl;
    }
    ofs.close();
    for (uint32_t i = 0; i < 4; i++)
    {
        std::ofstream of;
        of.open(SAVE_FILE + std::to_string(i));
        for (const SimEntry& se : entries)
        {
            if (se.res == i)
                of << to_u(se.loc.x) << " " << to_dt(se.loc.y) << endl;
        }
        of.close();
    }
}

void load_entries()
{
    if (!std::filesystem::exists(SAVE_FILE))
        return;
    std::ifstream ifs;
    ifs.open(SAVE_FILE);
    uint32_t n;
    ifs >> n;
    entries.reserve(n);
    for (uint32_t i = 0; i < n; i++)
    {
        double u, dt;
        uint32_t sr;
        ifs >> u >> dt >> sr;
        entries.push_back(SimEntry(vec2d(to_x(u), to_y(dt)), SimResult(sr)));
    }
    cout << "loaded " << n << " entries" << endl;
    ifs.close();
}

constexpr uint32_t stepsperframe = 10;

void run()
{
    const std::string CONTROL_FILE = "stop.comm";

    load_entries();

    while (!std::filesystem::exists(CONTROL_FILE))
    {
        for (uint32_t i = 0; i < 10; i++)
        {
            add_entry();
        }
    }
    std::filesystem::remove(CONTROL_FILE);

    save_entries();
}

}

int main()
{
    brandy0::run();
    return EXIT_SUCCESS;
}