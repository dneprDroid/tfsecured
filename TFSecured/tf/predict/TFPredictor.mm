//
//  TFPredictor.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import "TFPredictor.h"
#import "NSError+Util.h"
#import "Utils.h"

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>

using namespace tensorflow;

@interface TFPredictor () {
    GraphDef *graph;
    std::string inNode;
    std::string outNode;
}

@property(copy, nonatomic) NSString *modelPath;

@end

@implementation TFPredictor

+ (instancetype)initWith:(NSString*)modelPath
           inputNodeName:(NSString*)inNode
          outputNodeName:(NSString*)outNode {
    
    TFPredictor *pred = [TFPredictor new];
    pred.modelPath = modelPath;
    pred->inNode = "";
    pred->outNode = "";
    return pred;
}


- (void)loadModel:(nullable TFErrorCallback) callback {
    const char * path = [self.modelPath cStringUsingEncoding: NSUTF8StringEncoding];
    auto status = ReadBinaryProto(tensorflow::Env::Default(), path, graph);
    if (!status.ok()) {
        printf("Error reading graph: %s\n", status.error_message().c_str());
        if (callback)
            callback([NSError withMsg: @"Error reading graph"
                                 code: status.code()
                            localized: NSStringFromCString(status.error_message().c_str())]);
        return;
    }
}


//- (void)predictImage:(UIImage*)image {
//    Tensor in;
//    toTensor(image, &in);
//    [self predictTensor: in];
//}

- (void)predictTensor:(const Tensor&)input {
    SessionOptions options;
    Status status;
    std::unique_ptr<Session> session(NewSession(options));
    if (!status.ok()) {
        printf("Error creating session: %s\n", status.error_message().c_str());
        return;
    }
    
    status = session->Create(*graph);
    if (!status.ok()) {
        printf("Error adding graph to session: %s\n", status.error_message().c_str());
        return;
    }
    printf("Tensorn input shape: %s", input.shape().DebugString().c_str());
    
    std::vector<tensorflow::Tensor> outputs;
    status = session->Run({{inNode, input}},
                          {outNode},
                          {},
                          &outputs);
    if (!status.ok()) {
        return;
    }
    if (outputs.size() == 0) {
        std::cout << "Outputs is empty!" << "\n";
        return;
    }
    const tensorflow::Tensor &outTensor = outputs[0];
    
}


- (void)dealloc {
    delete graph;
}

@end
