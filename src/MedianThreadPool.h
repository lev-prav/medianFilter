//
// Created by lev on 22.05.23.
//

#pragma once

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <functional>

namespace Filters{

    class MedianThreadPool {
    public:

    private:
        std::function<void(cv::Mat)> workFunction;
    };
}
