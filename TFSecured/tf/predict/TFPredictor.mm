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
#include "../encryption/picosha2.hpp"
#include "../encryption/aes.hpp"

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
    
    auto status = ReadBinaryProto(tensorflow::Env::Default(), path, &graph);
    if (!status.ok()) {
        printf("Error reading graph: %s\n", status.error_message().c_str());
        if (callback)
            callback([NSError withMsg: @"Error reading graph"
                                 code: status.code()
                            localized: NSStringFromCString(status.error_message().c_str())]);
        return;
    }



    // Begin decryption:
    const std::string KEY = "JREH79XW7QKGX346LKU8MRM9SYM998";
    std::array<uint8_t, 32> hashKey;

    picosha2::hash256_bytes(KEY, hashKey);
    
    
    #define AES_block_size 16
    
    for (NodeDef& node : *graph.mutable_node()) {
        
        if (node.op() != "Const") continue;
        auto attr = node.mutable_attr();
        if (attr->count("value") == 0) continue;
        
        auto mutable_tensor = attr->at("value").mutable_tensor();
        const std::string &content = mutable_tensor->tensor_content();
        const uint32_t content_size = (uint32_t)mutable_tensor->ByteSizeLong();
        
        
        AES_ctx _aesCtx;
        std::vector<uint8_t> iv_bytes(content.begin(), content.begin() + AES_block_size);
        std::cout << "iv_bytes size: " << iv_bytes.size() << std::endl;
        AES_init_ctx_iv(&_aesCtx, hashKey.data(), iv_bytes.data());
        
        std::vector<uint8_t> bytes(content.begin() + AES_block_size, content.end());
        AES_CBC_decrypt_buffer(&_aesCtx, bytes.data(), content_size-AES_block_size);
        const size_t byte_size = bytes.size();
        size_t index = byte_size - (int)bytes[byte_size - 1];
        mutable_tensor->set_tensor_content(bytes.data(), index);
        std::cout   << "Node: " << node.name()
                    << ",\n     op: " << node.op()
                    << "\n content size (" << content_size << "): " << content.size() << "\n";
    }
    // Save Model:
    //    std::fstream file;
    //    file.open("filename");
    //    bool success = graph.SerializeToOstream(&file);
    //    file.close();
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
