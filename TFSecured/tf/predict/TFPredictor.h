//
//  TFPredictor.h
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIImage.h>
#include "UtilHeaders.h"

#ifdef __cplusplus

#include <tensorflow/core/framework/tensor.h>

#endif

NS_ASSUME_NONNULL_BEGIN

typedef void(^TFErrorCallback)(NSError *error);

@interface TFPredictor : NSObject



+ (instancetype)initWith:(NSString*)modelPath
           inputNodeName:(NSString*)inNode
          outputNodeName:(NSString*)outNode;


- (void)loadModelWithKey:(NSString*)key  error:(nullable TFErrorCallback) callback

  NS_SWIFT_NAME(loadModel(key:error:));

#ifdef __cplusplus

- (void)predictTensor:(const tensorflow::Tensor&)input output: (tensorflow::Tensor*)output;

#endif

@end

NS_ASSUME_NONNULL_END
