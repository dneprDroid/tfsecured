//
//  AESEncryptor.hpp
//  TFSecured
//
//  Created by user on 6/8/18.
//  Copyright © 2018 user. All rights reserved.
//

#include <stdio.h>


#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/framework/shape_inference.h>
#include <tensorflow/core/framework/tensor.h>

#include "aes.hpp"


    
class AESEncryptor {
public:
    AESEncryptor(const std::string &key);
    ~AESEncryptor();
    
    template<typename In>
    const tensorflow::Tensor encrypt(const tensorflow::Tensor& input);
    
    template<typename In>
    const tensorflow::Tensor decrypt(const tensorflow::Tensor& input);

private:
    const std::string _key;
    struct AES_ctx _aesCtx;
};
    
    
