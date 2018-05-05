//
//  Utils.h
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIImage.h>


#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/framework/shape_inference.h>


extern NSString* NSStringFromCString(const char *str);

void toTensor(UIImage *img, tensorflow::Tensor *outTensor);


void getBytes(UIImage *in, std::vector<unsigned char> &imageBytes);
