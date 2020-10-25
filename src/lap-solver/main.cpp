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
#include "painter.h"
#include "statgen.h"

namespace brandy0
{

constexpr uint32_t stepsperframe = 10;

void run()
{
    Simulator sim;
    sim.init();
    /*sim.iter();
    return;*/
    for (uint32_t i = 0;; i++)
    {
        save(sim, std::to_string(i) + ".png");
        print_stats(sim, i);
        for (uint32_t j = 0; j < stepsperframe; j++)
        {
            sim.iter();
        }
    }
}

}

int main()
{
    brandy0::run();
    return EXIT_SUCCESS;
}