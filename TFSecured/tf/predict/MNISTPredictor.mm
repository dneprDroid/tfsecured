//
//  MNISTPredictor.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import "MNISTPredictor.h"

#import "NSError+Util.h"
#import "Utils.h"


#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>

using namespace tensorflow;


@implementation MNISTPredictor

- (void)predictImage:(UIImage*)image {
    Tensor in;
    toTensor(image, &in);
    Tensor out;
    [self predictTensor:in output:&out];
}


@end
