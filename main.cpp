#include <iostream>
#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <thread>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "src/MedianFilter.h"
#include "opencv2/core/core.hpp"


#define PRINT 0

cv::Mat test(const cv::Mat& rawImg, int apertureSize){
    Filters::MedianFilter filter(apertureSize);

    //cv::Mat dst_;
    auto begin = std::chrono::steady_clock::now();
    auto smooth = filter.smoothImage(rawImg);
    auto end = std::chrono::steady_clock::now();


    double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout <<"Aperture " <<apertureSize<<"x"<<apertureSize<<"; Time difference = " << elapsedTime << "[ms]" << std::endl;
    std::cout<< 1000.0/elapsedTime<<" FPS\n";

    return smooth;
}

int main() {
    std::string image_path = "../res/maxresdefault.jpg";
    auto src = imread( image_path, cv::IMREAD_GRAYSCALE);
    if (src.empty())
    {
        std::cout<<"Can't open image :"<<image_path<<"\n";
        return EXIT_FAILURE;
    }

    auto smoothImg = test(src, 5);

    std::cout<<std::thread::hardware_concurrency()<<"\n";
    cv::imshow("Original", src);
    cv::imshow("Smoothed", smoothImg);
    cv::waitKey();

    return 0;
}
