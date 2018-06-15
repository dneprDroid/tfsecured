//
//  TFPredictor.m
//  TFSecured
//
//  Created by user on 5/3/18.
//  Copyright © 2018 user. All rights reserved.
//

#import "TFPredictor.h"
#import "NSError+Util.h"
#import "Utils.hpp"

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>
#include <iostream>
#include <fstream>
#include "../encryption/GraphDefDecryptor.hpp"
#include "../encryption/OpDecryptor.hpp"

using namespace tensorflow;

@interface TFPredictor () {
    GraphDef graph;
    std::string inNode;
    std::string outNode;
}

@property(copy, nonatomic) NSString *modelPath;

@end

@implementation TFPredictor

+ (instancetype)initWith:(NSString*)modelPath
           inputNodeName:(NSString*)inNode
          outputNodeName:(NSString*)outNode {
    
    TFPredictor *pred = [self new];
    pred.modelPath = modelPath;
    pred->inNode  = std::string([inNode cStringUsingEncoding:NSUTF8StringEncoding]);
    pred->outNode = std::string([outNode cStringUsingEncoding:NSUTF8StringEncoding]);
    return pred;
}


- (void)loadModel:(nullable TFErrorCallback) callback {
    const char * path = [self.modelPath cStringUsingEncoding: NSUTF8StringEncoding];
    std::cout << "Loading pb model from path: " << path << std::endl;
    
    auto status = tf_secured::GraphDefDecrypt(tensorflow::Env::Default(), path, &graph, "JREH79XW7QKGX346LKU8MRM9SYM998");
    if (!status.ok()) {
        printf("Error reading graph: %s\n", status.error_message().c_str());
        if (callback)
            callback([NSError withMsg: @"Error reading graph"
                                 code: status.code()
                            localized: NSStringFromCString(status.error_message().c_str())]);
        return;
    }
}

- (void)predictTensor:(const Tensor&)input output: (Tensor*)output {
    SessionOptions options;
    Status status;
    std::unique_ptr<Session> session(NewSession(options));
    if (!status.ok()) {
        printf("Error creating session: %s\n", status.error_message().c_str());
        return;
    }
    
    status = session->Create(graph);
    if (!status.ok()) {
        printf("Error adding graph to session: %s\n", status.error_message().c_str());
        return;
    }
    std::cout << "Tensor input shape: " << input.shape().DebugString() << "\n";
    
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
    *output = outputs[0];
}


- (void)dealloc {
    printf("...... dealloc ......\n");
}

@end
