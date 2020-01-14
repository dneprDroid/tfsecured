#include "img2tensor.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

void fillTensor(const std::string &path, int w, int h, float *tensor) {
    Mat img = imread(path, IMREAD_COLOR);
    Mat grey;
    cvtColor(img, grey, COLOR_BGR2GRAY);
    Mat tensorImg(h, w, CV_32FC1, tensor);
    grey.convertTo(tensorImg, CV_32FC1);
}