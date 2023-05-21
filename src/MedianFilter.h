//
// Created by lev on 20.05.23.
//

#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

namespace Filters{
    class MedianFilter {
    public:
        explicit MedianFilter(uint apertureSize) : apertureSize_(apertureSize)
        {
            window_.resize(apertureSize_*apertureSize);
        }

        cv::Mat smoothSignal(const cv::Mat& inputImage);

    private:

        int validateSignal(const cv::Mat& inputImage);
        cv::Mat expandMat(const cv::Mat& inputImage);

        int apertureSize_;
        std::vector<uchar> window_;
    };
};

