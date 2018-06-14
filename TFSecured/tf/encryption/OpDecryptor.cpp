//
//  OpDecryptor.cpp
//  TFSecured
//
//  Created by user on 6/14/18.
//  Copyright © 2018 user. All rights reserved.
//

#include "OpDecryptor.hpp"

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>
#include <iostream>
#include <fstream>
#include "GraphDefDecryptor.hpp"


using namespace tensorflow;
using namespace tf_secured;
using namespace register_kernel;

struct KernelRegistration {
    KernelRegistration(const KernelDef& d, StringPiece c,
                       kernel_factory::OpKernelRegistrar::Factory f)
    : def(d), kernel_class_name(c.ToString()), factory(f) {}
    const KernelDef def;
    const string kernel_class_name;
    const kernel_factory::OpKernelRegistrar::Factory factory;
};

typedef std::unordered_multimap<string, KernelRegistration> KernelRegistry;

static KernelRegistry* GlobalKernelRegistryTyped() {
    return reinterpret_cast<KernelRegistry*>(GlobalKernelRegistry());
}

static string generateKey(const StringPiece &op_type,
                          const DeviceType  &device_type,
                          const StringPiece &label) {
    return strings::StrCat(op_type, ":", device_type.type(), ":", label);
}



void tf_secured::DecryptOpsNames(tensorflow::GraphDef &graph,
                                 const std::array<uint8_t, 32> &keyByteArray) {
    
    const auto registryTyped = GlobalKernelRegistryTyped();
    const size_t node_size = graph.node_size();
    std::map<std::string, std::string> node_map;
    AES_ctx ctx;
    for (NodeDef& node : *graph.mutable_node()) {
        const std::string op_name = node.op();
        const size_t name_size = op_name.size();
        const auto bytes = tf_secured::decrypt(&ctx, keyByteArray, op_name, (uint32_t)name_size);
        const std::string decrypted_name(reinterpret_cast<const char*>(bytes.data()),
                                         bytes.size());
        node_map[op_name] = decrypted_name;
    }
    for (auto iterator = registryTyped->begin(); iterator != registryTyped->end(); ++iterator) {
        
        std::cout << "Node Reg Key: " << iterator->first << std::endl;
    }
}



void tf_secured::DecryptOpsNames(tensorflow::GraphDef &graph, const std::string &key256) {
    std::array<uint8_t, 32> hashKey;
    
    picosha2::hash256_bytes(key256, hashKey);
    tf_secured::DecryptOpsNames(graph, hashKey);
}
