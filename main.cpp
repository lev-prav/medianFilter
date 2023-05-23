#include <iostream>
#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <thread>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "src/MedianFilter.h"
#include "opencv2/core/core.hpp"


#define PRINT 0

void sort(std::vector<uchar>& window, uchar value){

}

int medianBlur(const cv::Mat& inputImage, cv::Mat& smoothedImage, int apertureSize){
    if (inputImage.empty() or smoothedImage.empty()){
        std::cout<<"Empty image is not allowed !\n";
        return EXIT_FAILURE;
    }

    if (apertureSize < 0 or  apertureSize % 2 == 0 or
        apertureSize >= inputImage.cols or apertureSize >= inputImage.rows){
        std::cout<<"Wrong apertureSize!\n";
        return EXIT_FAILURE;
    }


    std::vector<uchar> window(apertureSize*apertureSize);


    auto* imgMat = inputImage.ptr();
    auto* smoothMat = smoothedImage.ptr();
    auto smoothWidth = smoothedImage.cols;

    for(int row = apertureSize/2; row < inputImage.rows - apertureSize/2; row++){
        for(int col = apertureSize/2; col < inputImage.cols - apertureSize/2; col++){
            cv::Point pixelPosition(row,col),
            windowPosition(col - apertureSize/2, row - apertureSize/2);

            for(int i = 0; i < apertureSize; i++){
                for(int j = 0; j < apertureSize; j++){
                    window[i*apertureSize + j] = imgMat[(windowPosition.y + i)*inputImage.cols  + windowPosition.x + j];
                }
            }

            std::sort(window.begin(), window.end());
            auto median = window[window.size()/2];
#if PRINT
            for(auto n : window){
                std::cout<<int(n)<<" ";
            }
            std::cout<<" : "<<int(median)<<"\n";
#endif
            smoothMat[row*smoothWidth + col] = median;
        }
    }
    return EXIT_SUCCESS;
}

cv::Mat test(cv::Mat& src, int windowSize){
    //    src = cv::Mat(6,6, CV_8U);
//    cv::randu(src, 0, 256);

    cv::Mat dst = src.clone();
    std::cout<<src.size<<"\n";

#if PRINT

    auto* row = src.ptr();
    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            std::cout<<std::setw(12)<<int(row[i*src.cols + j])<<" ";
        }
        std::cout<<"\n";
    }
#endif
    auto begin = std::chrono::steady_clock::now();

    auto error = medianBlur(src, dst, windowSize);

    auto end = std::chrono::steady_clock::now();
    if (error != EXIT_SUCCESS){
        std::cout<<"! ERROR\n";
    }
    double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout <<"Aperture " <<windowSize<<"x"<<windowSize<<"; Time difference = " << elapsedTime << "[ms]" << std::endl;
    std::cout<< 1000.0/elapsedTime<<" FPS\n";
#if PRINT
    auto* row_dst = dst.ptr();
    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            std::cout<<std::setw(12)<<int(row_dst[i*src.cols + j])<<" ";
        }
        std::cout<<"\n";
    }
#endif
    cv::imshow("original", src);
    cv::imshow("color", dst);
    cv::waitKey();
    cv::destroyAllWindows();
    return dst;
}


int main() {
    std::string image_path = "../res/maxresdefault.jpg";
    auto src = imread( image_path, cv::IMREAD_GRAYSCALE);
    if (src.empty())
    {
        std::cout<<"Can't open image :"<<image_path<<"\n";
        return EXIT_FAILURE;
    }

    int windowSize = 3;
    Filters::MedianFilter filter(windowSize);

    //cv::Mat dst_;
    auto begin = std::chrono::steady_clock::now();
    auto dst_ = filter.smoothSignal(src);
    //cv::medianBlur(src, dst_, 5);
    auto end = std::chrono::steady_clock::now();

    double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout <<"Aperture " <<windowSize<<"x"<<windowSize<<"; Time difference = " << elapsedTime << "[ms]" << std::endl;
    std::cout<< 1000.0/elapsedTime<<" FPS\n";
    std::cout<<std::thread::hardware_concurrency()<<"\n";
    cv::imshow("Original", src);
    cv::imshow("Smoothed", dst_);
    cv::waitKey();

    return 0;
}
