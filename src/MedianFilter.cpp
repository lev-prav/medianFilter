//
// Created by lev on 20.05.23.
//

#include <iostream>
#include <iomanip>
#include <thread>
#include <future>
#include "MedianFilter.h"
#include "Histogram.h"


cv::Mat Filters::MedianFilter::smoothImage(const cv::Mat &inputImage) {
    int validationResult = validateSignal(inputImage);

    if (validationResult != EXIT_SUCCESS){
        return {};
    }

    std::vector<cv::Mat> channels(inputImage.channels()),
                         smoothChannels;
    cv::Mat smoothImage;

    cv::split(inputImage, channels);
    //TODO: ADD THREADS
//    int threadsNum = std::max(1, int(std::thread::hardware_concurrency()));

    auto filterSingleChannel = [](const cv::Mat& inputChannel, int apertureSize) -> cv::Mat{
        MedianFilter filter(apertureSize);
        return filter.smoothChannel(inputChannel);
    };

    std::vector<std::future<cv::Mat>> futures;
    for(cv::Mat& channel : channels){
        futures.emplace_back(
                std::async(std::launch::async, filterSingleChannel, channel, apertureSize_)
        );
    }

//    auto begin = std::chrono::steady_clock::now();

    for(auto& future : futures){
        smoothChannels.emplace_back(future.get());
    }

    // Costs about 5 ms
    cv::merge(smoothChannels, smoothImage);

    return smoothImage;
}

cv::Mat Filters::MedianFilter::smoothChannel(const cv::Mat &inputImage) {

    cv::Mat extendedImage = expandMat(inputImage);
    cv::Mat smoothImage = inputImage.clone();

    int threadsNum = std::max(1, int(std::thread::hardware_concurrency()));

    std::vector<std::future<void>> filtersThreads;

    //TODO: можно засунуть работу с потоками в тред пулл
    for(int startCol = 0, step = inputImage.cols/threadsNum, apertureSize = apertureSize_;
        startCol < inputImage.cols;
        startCol += step){

        filtersThreads.emplace_back(std::async(
                [&extendedImage, &smoothImage, &apertureSize](int startCol, int endCol) {

                    filterColumns(extendedImage, smoothImage, startCol, endCol, apertureSize);

                    }
        ,startCol, startCol + step));
    }

    for (auto& filterThread : filtersThreads) {
        filterThread.wait();
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
    //thickness of additional borders
    int expansionZone = apertureSize_/2;
    cv::Mat smoothTemplate = cv::Mat::zeros(inputImage.rows +  expansionZone*2, inputImage.cols + expansionZone*2, inputImage.type());

    cv::Rect inputImagePosition(expansionZone, expansionZone,inputImage.cols, inputImage.rows);

    auto inputImageArea = smoothTemplate(inputImagePosition);
    inputImage.copyTo(inputImageArea);


    // filling blank zone on sides
    for(int row = inputImagePosition.y; row < inputImagePosition.y + inputImagePosition.height; row++){
        auto smoothRowPtr = smoothTemplate.ptr(row);

        //fill blank left side
        std::memset(smoothRowPtr, smoothRowPtr[expansionZone] ,expansionZone);
        //fill blank right side
        std::memset(smoothRowPtr + inputImagePosition.x + inputImagePosition.width,
                    smoothRowPtr[ inputImagePosition.x + inputImagePosition.width - 1] ,expansionZone);
    }

    // get pointers to first and last filled row
    auto    smoothPtrTop = smoothTemplate.ptr(inputImagePosition.y),
            smoothPtrBottom = smoothTemplate.ptr(inputImagePosition.y + inputImagePosition.height - 1);

    for(int row = 0; row < expansionZone; row++){
        auto    smoothBlankTop = smoothTemplate.ptr(row),
                smoothBlankBottom = smoothTemplate.ptr(row + inputImagePosition.y + inputImagePosition.height);

        //filling border lines up
        std::memcpy(smoothBlankTop,smoothPtrTop,smoothTemplate.cols);
        //and down
        std::memcpy(smoothBlankBottom,smoothPtrBottom,smoothTemplate.cols);
    }

    return smoothTemplate;
}

void Filters::MedianFilter::filterColumns(const cv::Mat &extendedImage, cv::Mat &smoothImage, int firstCol, int lastCol, int apertureSize) {
    auto smoothPtr = smoothImage.ptr();
    int windowSize = apertureSize*apertureSize/2;

    std::vector<Histogram> histograms(lastCol - firstCol);

    for(int col = firstCol, histCol = 0; col < lastCol; col++, histCol++)
    {
        Histogram& columnHist = histograms.at(histCol);

        for (int windowRow = 0; windowRow < apertureSize; windowRow++) {
            auto extendedRow = extendedImage.ptr(windowRow);
            for(int windowCol = 0; windowCol < apertureSize; windowCol++){
                columnHist.increaseFrequency(extendedRow[col + windowCol]);
            }
        }

        for(int row = 0, nextRow = apertureSize; row < smoothImage.rows; row++, nextRow = std::min(smoothImage.rows, nextRow + 1))
        {

            uchar histInd = 0;
            for (int accum = 0; accum < windowSize/2; ++histInd){
                accum += columnHist.getCount(histInd);
            }
            smoothPtr[row*smoothImage.cols + col ] = histInd - 1;

            auto oldestRowPtr = extendedImage.ptr(row),
                 nextRowPtr = extendedImage.ptr(nextRow);

            for(int i = 0; i < apertureSize; i++){
                columnHist.reduceFrequency(   oldestRowPtr[col + i]);
                columnHist.increaseFrequency( nextRowPtr[col + i]);

            }

        }

    }

}
