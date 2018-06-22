//
//  MNISTPredictor.h
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//
#import "TFPredictor.h"

NS_ASSUME_NONNULL_BEGIN

enum {
    MNIST_IMAGE_NOT_RECOGNIZED = -1,
    MNIST_IMAGE_PIXEL_SIDE_SIZE = 28
};

typedef void(^MNISTSuccessCallback)(NSUInteger digit);
typedef void(^MNISTErrorCallback)();

@interface MNISTPredictor : TFPredictor

- (void)predictImage:(UIImage*)image
             success:(MNISTSuccessCallback)success
               error:(MNISTErrorCallback)error

NS_SWIFT_NAME(predict(image:success:error:));


@end

NS_ASSUME_NONNULL_END
