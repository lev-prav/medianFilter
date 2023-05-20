#include <iostream>
#include <iomanip>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#define PRINT 0

int medianBlur(const cv::Mat& inputImage, cv::Mat& smoothedImage, int apertureSize){
    if (apertureSize < 0 or  apertureSize % 2 == 0){
        std::cout<<"Positive Odd number needed\n";
        return EXIT_FAILURE;
    }

    std::vector<uchar> window(apertureSize*apertureSize);


    auto* imgMat = inputImage.ptr();

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
            smoothedImage.at<uchar>(row,col) = median;
        }
    }
    return EXIT_SUCCESS;
}



int main() {
    std::string image_path = "../res/maxresdefault.jpg";
    auto src = imread( image_path, cv::IMREAD_GRAYSCALE);
//    src = cv::Mat(6,6, CV_8U);
//    cv::randu(src, 0, 256);

    cv::Mat dst = src.clone();
    if (src.empty())
    {
        std::cout<<"Can't open image :"<<image_path<<"\n";
        return EXIT_FAILURE;
    }
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
    auto error = medianBlur(src, dst, 7);
    if (error != EXIT_SUCCESS){
        std::cout<<"! ERROR\n";
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

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

    return 0;
}
