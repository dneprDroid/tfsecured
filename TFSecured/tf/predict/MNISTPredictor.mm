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

using namespace tensorflow;


@implementation MNISTPredictor


- (NSUInteger)predictImage:(UIImage*)image {
    Tensor in;
    toTensor(image, &in);
    std::cout << "Input tensor shape: " << in.shape().DebugString() << "\n";
    Tensor out;
    [self predictTensor:in output:&out];
    std::cout << "Output tensor shape: " << out.shape().DebugString() << "\n";
    auto flatTensor = out.flat<float>();
    return (NSUInteger)flatTensor(0);
}


@end
