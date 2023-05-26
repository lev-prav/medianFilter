//
// Created by lev on 20.05.23.
//

#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

namespace Filters{
    class MedianFilter {
    public:
        explicit MedianFilter(uint apertureSize) : apertureSize_(int(apertureSize))
        {
            window_.resize(apertureSize_*apertureSize);
        }

        cv::Mat smoothImage(const cv::Mat& inputImage);

    private:

        cv::Mat smoothChannel(const cv::Mat& inputImage);

        int validateSignal(const cv::Mat& inputImage);
        cv::Mat expandMat(const cv::Mat& inputImage);
        static void filterLines(const cv::Mat& smoothTemplate, cv::Mat& image, int firstCol, int lastCol, int apertureSize);

        int apertureSize_;
        std::vector<uchar> window_;
    };
};

