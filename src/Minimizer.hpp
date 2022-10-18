#pragma once

#include "Dfa.hpp"

class Minimizer {
    static const int KONSTANTA = 41;
public:
    static Dfa *minimize(Dfa *dfa);
};