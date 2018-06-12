//
//  AESEncryptor.cpp
//  TFSecured
//
//  Created by user on 6/8/18.
//  Copyright © 2018 user. All rights reserved.
//

#include "AESEncryptor.hpp"


template<typename Src, typename Dest>
inline Dest* __convertData(const Src* data, const size_t elmCount, size_t *returnCount){
    
    const size_t retCount  = elmCount * sizeof(Src) / sizeof(Dest);
    Dest *result = (Dest*)calloc(retCount, sizeof(Dest));
    memcpy(result, data, retCount * sizeof(Dest));
    if (returnCount) {
        *returnCount = retCount;
    }
    return result;
}

inline size_t tensorElmCount(const tensorflow::Tensor &tensor){
    size_t size = 0;
    auto shape = tensor.shape();
    size = shape.dims();
    return size;
}

AESEncryptor::AESEncryptor(const std::string &key)
             : _key(key) {
     AES_init_ctx(&_aesCtx, (const uint8_t*)key.c_str());
}

AESEncryptor::~AESEncryptor() {

}




template<typename In>
const tensorflow::Tensor AESEncryptor::encrypt(const tensorflow::Tensor& input) {
    tensorflow::Tensor out(input.dtype(), input.shape());

    const size_t elmCount = tensorElmCount(input);
    size_t bytesCount = 0;
    auto tensorBytes = input.flat<In>().data();
    uint8_t *bytes = __convertData<In, uint8_t>(tensorBytes, elmCount, &bytesCount);
    AES_ECB_encrypt(&_aesCtx, bytes);
    tensorBytes = __convertData<uint8_t, In>(bytes, bytesCount,  nullptr);
    return out;
}

template<typename In>
const tensorflow::Tensor AESEncryptor::decrypt(const tensorflow::Tensor& input) {
    tensorflow::Tensor out(input.dtype(), input.shape());
    const size_t tensorElmCount = input.AllocatedBytes()/sizeof(In);
    size_t bytesCount = 0;
    uint8_t *bytes = __convertData<In, uint8_t>(input.flat<In>().data(), tensorElmCount, &bytesCount);
    AES_ECB_decrypt(&_aesCtx, bytes);
    return out;
}

