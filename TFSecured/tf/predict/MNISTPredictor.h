//
//  MNISTPredictor.h
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//
#import "TFPredictor.h"

NS_ASSUME_NONNULL_BEGIN


@interface MNISTPredictor : TFPredictor

- (NSUInteger)predictImage:(UIImage*)image

   NS_SWIFT_NAME(predict(image:));


@end

NS_ASSUME_NONNULL_END
