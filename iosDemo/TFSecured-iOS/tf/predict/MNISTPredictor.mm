//
//  MNISTPredictor.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import "MNISTPredictor.h"

#import "NSError+Util.h"
#import "Utils.hpp"


#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>


template <typename T, tensorflow::DataType Type>
void toGrayscaleTensor(const std::vector<unsigned char> &imageBytes,
                       const long height,
                       const long width,
                       const int channels,
                       tensorflow::Tensor *outTensor) {
    *outTensor = tensorflow::Tensor(Type,
                                    tensorflow::TensorShape({1, height * width}));
    auto input_tensor_mapped = outTensor->tensor<T, 2>();
    const tensorflow::uint8*  source_data = imageBytes.data();
    for (int y = 0; y < height; ++y) {
        const tensorflow::uint8* source_row = source_data + (y * width * channels);
        
        for (int x = 0; x < width; ++x) {
            const tensorflow::uint8* source_pixel = source_row + (x * channels);
            
            T r = source_pixel[2];
            T g = source_pixel[1];
            T b = source_pixel[0];
            
            T gray = (r + g + b)/3/255;
            long pos = y * width + x;
            
//#if DEBUG
//            std::cout   << "Pixel at " << pos << " ("
//                        << r << ", "
//                        << g << ", "
//                        << b << ") => "
//                        <<  gray  << "\n";
//#endif
            
            input_tensor_mapped(pos) = gray;
        }
    }
}


using namespace tensorflow;


@implementation MNISTPredictor


- (void)predictImage:(UIImage*)image
             success:(MNISTSuccessCallback)success
               error:(MNISTErrorCallback)error {
    const long width = image.size.width;
    const long height = image.size.height;
    
    NSAssert(width == height && width == MNIST_IMAGE_PIXEL_SIDE_SIZE,
             @"Invalid image input size : %lu (must be %i)", width, MNIST_IMAGE_PIXEL_SIDE_SIZE);
    
    std::vector<unsigned char> imageBytes(height * width * 4);
    getBytes(image, imageBytes);
    Tensor in;
    toGrayscaleTensor<float, DataType::DT_FLOAT>(imageBytes,
                                                 height,
                                                 width,
                                                 4,
                                                 &in);
    std::cout << "Input tensor shape: " << in.shape().DebugString() << "\n";
    Tensor out;
    [self predictTensor:in output:&out];
    
    std::cout << "Output tensor shape: " << out.shape().DebugString() << "\n";
    
    auto flatTensor = out.flat<float>();
    NSInteger digit = MNIST_IMAGE_NOT_RECOGNIZED;
    float digitProb = 0;
    
    for (int i=0; i < flatTensor.size(); ++i) {
        float prob = flatTensor(i);
        std::cout << "Probability for digit " << (i + 1) << " = " << prob << "\n";
        if (prob > digitProb) {
            digitProb = prob;
            digit = (i + 1);
        }
    }
    if (digit == MNIST_IMAGE_NOT_RECOGNIZED) {
        error();
        return;
    }
    success(digit);
    return;
}

- (void)dealloc {
    printf("MNISTPredictor dealloc.\n");
}

@end
