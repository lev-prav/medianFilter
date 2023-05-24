//
// Created by lev on 20.05.23.
//

#include <iostream>
#include <iomanip>
#include <thread>
#include "MedianFilter.h"


cv::Mat Filters::MedianFilter::smoothSignal(const cv::Mat &inputImage) {

    int validationResult = validateSignal(inputImage);

    if (validationResult != EXIT_SUCCESS){
        return {};
    }

    cv::Mat extendedImage = expandMat(inputImage);

    cv::Mat smoothImage = inputImage.clone();

    int threadsNum = std::max(1, int(std::thread::hardware_concurrency()));
    threadsNum = 1;
    std::vector<std::thread> threads;

    int step = inputImage.cols/threadsNum;
    int apertureSize = apertureSize_;


    for(int startCol = 0; startCol < inputImage.cols; startCol += step){
        threads.emplace_back([&extendedImage, &smoothImage, apertureSize](int startCol, int endCol){

                filterLines(extendedImage,smoothImage, startCol, endCol, apertureSize);

            },startCol, startCol + step);
    }


    for(auto& thread : threads){
        thread.join();
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

void Filters::MedianFilter::filterLines(const cv::Mat &extendedImage, cv::Mat &smoothImage, int firstCol, int lastCol, int apertureSize) {
    auto smoothPtr = smoothImage.ptr();
    int windowSize = apertureSize*apertureSize/2;

    std::vector<std::array<int, 256>> hist(lastCol - firstCol);


    for(int col = firstCol, histCol = 0; col < lastCol; col++, histCol++)
    {
        auto* hist_ = hist[histCol].data();

        for (int windowRow = 0; windowRow < apertureSize; windowRow++) {
            auto extendedRow = extendedImage.ptr(windowRow);
            for(int windowCol = 0; windowCol < apertureSize; windowCol++){
                hist_[extendedRow[col + windowCol]]++;
            }
        }

        for(int row = 0, windowRow = apertureSize - 1; row < smoothImage.rows; row++, windowRow++)
        {

            uchar histInd = 0;
            for (int accum = 0; accum < windowSize/2; histInd++){
                accum += hist_[histInd];
            }
            smoothPtr[row*smoothImage.cols + col ] = histInd - 1;

            auto extendedRow = extendedImage.ptr(row),
                 newRow = extendedImage.ptr(row + apertureSize);


            for(int i = 0; i < apertureSize; i++){
                hist_[extendedRow[col + i]]--;
                hist_[newRow[col + i]]++;
            }

        }

    }

}
