/**
 * partialders.h
 * 
 * Author: Viktor Fukala
 * Created on 2020/10/3
 */
#ifndef PARTIALDERS_H
#define PARTIALDERS_H

#include <vector>

#include "linearvar.h"

namespace brandy0
{

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

}

#endif // PARTIALDERS_H