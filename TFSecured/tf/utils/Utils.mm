//
//  Utils.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import "Utils.hpp"
#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/framework/shape_inference.h>


NSString* NSStringFromCString(const char *str) {
    return [[NSString alloc]initWithUTF8String:str];
}

void getBytes(UIImage *in, std::vector<unsigned char> &imageBytes) {
    CGImageRef ref = [in CGImage];
    const int width = (int)CGImageGetWidth(ref);
    const int height = (int)CGImageGetHeight(ref);
    const int channels = 4;
    const int bytes_per_row = (width * channels);
    const int bytes_in_image = (bytes_per_row * height);
    const int bits_per_component = 8;

    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
  
    CGContextRef context = CGBitmapContextCreate(imageBytes.data(),
                                                 width,
                                                 height,
                                                 bits_per_component,
                                                 bytes_per_row, color_space,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(color_space);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), ref);
    CGContextRelease(context);
//    CFRelease(ref);
}

void toTensor(UIImage *img, tensorflow::Tensor *outTensor) {
    CGImageRef ref = [img CGImage];
    const int width = (int)CGImageGetWidth(ref);
    const int height = (int)CGImageGetHeight(ref);
    const int channels = 4;
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    const int bytes_per_row = (width * channels);
    const int bytes_in_image = (bytes_per_row * height);
    std::vector<tensorflow::uint8> imageBytes(bytes_in_image);
    const int bits_per_component = 8;
    CGContextRef context = CGBitmapContextCreate(imageBytes.data(),
                                                 width,
                                                 height,
                                                 bits_per_component,
                                                 bytes_per_row, color_space,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(color_space);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), ref);
    CGContextRelease(context);
    CFRelease(ref);
    
    *outTensor = tensorflow::Tensor(tensorflow::DT_FLOAT,
                                    tensorflow::TensorShape({1, height, width, 3}));
    auto input_tensor_mapped = outTensor->tensor<float, 4>();
    const tensorflow::uint8 * source_data = imageBytes.data();
    
    for (int y = 0; y < height; ++y) {
        const tensorflow::uint8* source_row = source_data + (y * width * channels);
        
        for (int x = 0; x < width; ++x) {
            const tensorflow::uint8* source_pixel = source_row + (x * channels);
            
            float r = source_pixel[2];
            float g = source_pixel[1];
            float b = source_pixel[0];
            
            
            input_tensor_mapped(0, y, x, 0) = r;
            input_tensor_mapped(0, y, x, 1) = g;
            input_tensor_mapped(0, y, x, 2) = b;
        }
    }
}
