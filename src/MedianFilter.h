//
// Created by lev on 20.05.23.
//

#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

namespace Filters{
    class MedianFilter {
    public:
        explicit MedianFilter(uint apertureSize);

        cv::Mat smoothImage(const cv::Mat& inputImage);

    private:

        explicit MedianFilter(int apertureSize) : apertureSize_(apertureSize)
        {}

        cv::Mat smoothChannel(const cv::Mat& inputImage);

        bool checkAperture(uint aperture);

        int validateSignal(const cv::Mat& inputImage);
        cv::Mat expandMat(const cv::Mat& inputImage);
        static void filterColumns(const cv::Mat& extendedImage, cv::Mat& smoothImage, int firstCol, int lastCol, int apertureSize);

        int apertureSize_;
        int defaultAperture_ = 3;
    };
};

