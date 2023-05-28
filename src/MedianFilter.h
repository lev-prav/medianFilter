//
// Created by lev on 20.05.23.
//

#pragma once
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

namespace Filters{
    /*!
     * Implementation of Median Filter for 8 bit multichannel image (usually) realtime
     */
    class MedianFilter {
    public:
        /*!
         * User constructor for creating Median filter.
         * Has input validation.
         * @param apertureSize - side of square window filter
         */
        explicit MedianFilter(uint apertureSize);

        /*!
         * Start filter pipeline
         * @param inputImage
         * @return smooth (filtered) image
         */
        cv::Mat smoothImage(const cv::Mat& inputImage);

    private:
        /*!
         * Special private constructor without validation
         * @param apertureSize
         */
        explicit MedianFilter(int apertureSize) : apertureSize_(apertureSize)
        {}
        /*!
         * In case of multichannel images, this function has deal with a single
         * and process it asynchronously
         * @param inputImage
         * @return smoothChannel
         */
        cv::Mat smoothChannel(const cv::Mat& inputImage);
        /*!
         * Validate user defined filter size
         * @param aperture
         * @return
         */
        bool checkAperture(uint aperture);

        /*!
         * Check if the input image is correct
         * @param inputImage
         * @return
         */
        int validateSignal(const cv::Mat& inputImage) const;
        /*!
         * Supplements original image with duplicate borders
         * @param inputImage
         * @return
         */
        cv::Mat expandMat(const cv::Mat& inputImage) const;
        /*!
         * Performs filtering itself for specified part of the image
         * Filter goes from firstCol to lastCol in input image coordinates
         * @param extendedImage
         * @param smoothImage
         * @param firstCol
         * @param lastCol
         * @param apertureSize
         */
        static void filterColumns(const cv::Mat& extendedImage, cv::Mat& smoothImage, int firstCol, int lastCol, int apertureSize);

        int apertureSize_;
        int defaultAperture_ = 3;
    };
};

