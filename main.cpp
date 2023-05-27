#include <iostream>
#include <iomanip>
#include <opencv2/imgproc.hpp>
#include <thread>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "src/MedianFilter.h"
#include "opencv2/core/core.hpp"
#include <fstream>
#include <map>

#define PRINT 0

cv::Mat test(const cv::Mat& rawImg, int apertureSize, double& time,std::stringstream & logFile){
    Filters::MedianFilter filter(apertureSize);

    //cv::Mat dst_;
    auto begin = std::chrono::steady_clock::now();
    auto smooth = filter.smoothImage(rawImg);
    auto end = std::chrono::steady_clock::now();


    double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout <<"Aperture " <<apertureSize<<"x"<<apertureSize<<"; Time difference = " << elapsedTime << "[ms]" << std::endl;
    std::cout<< 1000.0/elapsedTime<<" FPS\n";

    logFile<<"Aperture " <<apertureSize<<"x"<<apertureSize<<"; Time difference = " << elapsedTime << "[ms]" << std::endl;
    logFile<< 1000.0/elapsedTime<<" FPS\n";
    time = elapsedTime;

    return smooth;
}

int main() {
    std::string image_path = "../res/maxresdefault.jpg";
    auto src = imread( image_path, cv::IMREAD_COLOR);

    if (src.empty())
    {
        std::cout<<"Can't open image :"<<image_path<<"\n";
        return EXIT_FAILURE;
    }

    cv::Mat graySrc;
    cv::cvtColor(src, graySrc,  cv::COLOR_BGR2GRAY);

    int testCases = 10;
    std::vector<int> apertures = {3, 5};
    std::stringstream benchmarks;

    std::map<std::string, cv::Mat> imgData {
            {"GRAY", graySrc},
            {"COLOR", src}
    };

    for (auto [imgType, inputImg] : imgData) {
        benchmarks<<imgType<<" size: "<<inputImg.rows<<"x"<<inputImg.cols<<"\n";
        std::cout<<imgType<<" size: "<<inputImg.rows<<"x"<<inputImg.cols<<"\n";

        cv::Mat smoothImg;

        for(int aperture : apertures){
            double timePassed{0}, totalTime{0}, meanTime{0}, fps{0};
            for(int i = 0; i < testCases; i++){
                smoothImg = test(inputImg, aperture, timePassed, benchmarks);
                totalTime += timePassed;
            }
            meanTime = totalTime/testCases;
            fps = 1000.f/meanTime;

            std::cout<<"Mean time: "<<meanTime<<"[ms]\n";
            std::cout<<"Mean fps: "<<fps<<" FPS\n";

            benchmarks<<"Mean time: "<<meanTime<<"[ms]\n";
            benchmarks<<"Mean fps: "<<fps<<" FPS\n";


//            cv::imshow("Original", inputImg);
//            cv::imshow("Smoothed", smoothImg);
//            cv::waitKey();
//            cv::destroyAllWindows();
        }

        benchmarks<<"\n";
        std::cout<<"\n";
    }

    std::ofstream benchmarksFile("../benchmarks.txt");
    benchmarksFile<<benchmarks.str();

    return 0;
}
