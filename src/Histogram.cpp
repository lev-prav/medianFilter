//
// Created by lev on 24.05.23.
//

#include "Histogram.h"

void Filters::Histogram::add(int index) {
    histPtr_[index]++;
}

void Filters::Histogram::subtract(int index) {
    histPtr_[index]--;
}
