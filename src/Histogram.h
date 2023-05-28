//
// Created by lev on 24.05.23.
//

#pragma once

#include <array>

namespace Filters{
    using uchar = unsigned char;
    /*!
     * Frequency histogram
     */
    class Histogram {
    public:

        inline void increaseFrequency(int intensity) {
            histPtr_[intensity]++;
        }
        inline void reduceFrequency(int intensity){
            histPtr_[intensity]--;
        }
        inline int getCount(int intensity)  {
            return histPtr_[intensity];
        }


    private:
        std::array<uchar, 256> hist_ {0};
        //pointer for fast navigation
        uchar* histPtr_ = hist_.data();
    };
}
