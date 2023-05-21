//
// Created by lev on 20.05.23.
//

#include <iostream>
#include <iomanip>
#include "MedianFilter.h"


cv::Mat Filters::MedianFilter::smoothSignal(const cv::Mat &inputImage) {

    int validationResult = validateSignal(inputImage);

    if (validationResult != EXIT_SUCCESS){
        return {};
    }

    cv::Mat extendedImage = expandMat(inputImage);
    cv::Mat smoothImage = cv::Mat::zeros(inputImage.rows, inputImage.cols, inputImage.type());;
    //TODO: add border control
    auto windowPtr = window_.data();
    auto extendedPtr = extendedImage.ptr();
    auto smoothPtr = smoothImage.ptr();

    cv::Point windowPosition(0,0);

    for(int row = apertureSize_/2; row < extendedImage.rows - apertureSize_/2; row++, windowPosition.y++, windowPosition.x = 0){

        for(int col = apertureSize_/2; col < extendedImage.cols - apertureSize_/2; col++, windowPosition.x++) {

            for (int windowRow = 0; windowRow < apertureSize_; windowRow++) {
                std::memcpy(windowPtr + windowRow*apertureSize_,
                            extendedPtr + (windowPosition.y + windowRow)* extendedImage.cols + windowPosition.x, apertureSize_);
            }

            std::sort(window_.begin(), window_.end());

            auto median = window_[apertureSize_/ 2];
            smoothPtr[row*smoothImage.cols + col ] = median;
        }

    }
    return smoothImage;
}

int Filters::MedianFilter::validateSignal(const cv::Mat &inputImage) {
    if (inputImage.empty()){
        std::cout<<"Empty image is not allowed !\n";
        return EXIT_FAILURE;
    }

    if (apertureSize_ % 2 == 0
        or apertureSize_ >= inputImage.cols
        or apertureSize_ >= inputImage.rows){

        std::cout<<"Wrong apertureSize!\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

cv::Mat Filters::MedianFilter::expandMat(const cv::Mat &inputImage) {
    int expansionZone = apertureSize_/2;
    cv::Mat smoothTemplate = cv::Mat::zeros(inputImage.rows +  expansionZone*2, inputImage.cols + expansionZone*2, inputImage.type());

    cv::Rect inputImagePosition(expansionZone, expansionZone,inputImage.cols, inputImage.rows);

    auto inputImageArea = smoothTemplate(inputImagePosition);
    inputImage.copyTo(inputImageArea);


    // filling blank zone on sides
    for(int row = inputImagePosition.y; row < inputImagePosition.y + inputImagePosition.height; row++){
        auto* smoothPtr = smoothTemplate.ptr(row);

        //fill blank left side
        std::memset(smoothPtr, smoothPtr[expansionZone] ,expansionZone);
        //fill blank right side
        std::memset(smoothPtr + inputImagePosition.x + inputImagePosition.width, smoothPtr[ inputImagePosition.x + inputImagePosition.width - 1] ,expansionZone);
    }

    auto    smoothPtrForward = smoothTemplate.ptr(inputImagePosition.y),
            smoothPtrReverse = smoothTemplate.ptr(inputImagePosition.y + inputImagePosition.height - 1);

    for(int row = 0; row < expansionZone; row++){
        auto    smoothBlankTop = smoothTemplate.ptr(row),
                smoothBlankBottom = smoothTemplate.ptr(row + inputImagePosition.y + inputImagePosition.height);

        std::memcpy(smoothBlankTop,smoothPtrForward,smoothTemplate.cols);
        std::memcpy(smoothBlankBottom,smoothPtrReverse,smoothTemplate.cols);
    }

//    cv::imshow("template", smoothTemplate);
//    cv::waitKey();

    return smoothTemplate;
}

