#include "img2tensor.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using cv::Mat;

void fillTensor(const std::string &path, int w, int h, float *tensorPtr) {

    Mat img = cv::imread(path, cv::IMREAD_COLOR);
    Mat grey;
    cv::cvtColor(img, grey, cv::COLOR_BGR2GRAY);
    
    Mat tensorImg(h, w, CV_32FC1, tensorPtr);
    grey.convertTo(tensorImg, CV_32FC1);
}