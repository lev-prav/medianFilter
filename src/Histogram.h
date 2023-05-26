//
// Created by lev on 24.05.23.
//

#pragma once

#include <array>

namespace Filters{
    using uchar = unsigned char;

    class Histogram {
    public:

        inline void add(int index);
        inline void subtract(int index);

    private:
        std::array<uchar, 256> hist_ {0};
        uchar* histPtr_ = hist_.data();
    };
}
