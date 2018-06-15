//
//  GraphDefDecryptor.hpp
//  TFSecured
//
//  Created by user on 6/14/18.
//  Copyright © 2018 user. All rights reserved.
//

#include <stdio.h>

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>
#include <iostream>
#include <fstream>
#include "picosha2.hpp"
#include "aes.hpp"

using namespace tensorflow;

namespace tf_secured {

    #define DEFAULT_KEY_SIZE 32
    
    static const int32_t AES_BLOCK_SIZE       = AES_BLOCKLEN;
    static const int32_t AES_INIT_VECTOR_SIZE = AES_BLOCK_SIZE;
    
    namespace internal  {
        template<typename In>
        inline void decryptAES(const std::array<uint8_t, DEFAULT_KEY_SIZE> &keyByteArray,
                               std::vector<In> &input_content,
                               const uint32_t content_size);
    }
    
    typedef void (*Decryptor)(const std::array<uint8_t, DEFAULT_KEY_SIZE> &keyByteArray,
                              std::vector<char> &input_content,
                              const uint32_t content_size);
    
    inline Status GraphDefDecrypt(tensorflow::Env *env,
                                  const std::string &modelPath,
                                  GraphDef *graph,
                                  const std::array<uint8_t, DEFAULT_KEY_SIZE> &keyByteArray,
                                  const Decryptor decryptor) {
        
        std::ifstream file(modelPath, std::ios::binary | std::ios::ate);
        std::vector<char> bytes;
        if (!file.eof() && !file.fail()) {
            file.seekg(0, std::ios_base::end);
            std::streampos fileSize = file.tellg();
            bytes.resize(fileSize);
            
            file.seekg(0, std::ios_base::beg);
            file.read(&bytes[0], fileSize);
        } else {
            return errors::DataLoss("Can't read file at ", modelPath);
        }
        
        decryptor(keyByteArray, bytes, (uint32_t)file.tellg());
        
        if (!graph->ParseFromArray(bytes.data(), (int)bytes.size())) {
            return errors::DataLoss("Can't parse ", modelPath, " as binary proto");
        }
        return Status::OK();
    }
    
    inline Status GraphDefDecryptAES(tensorflow::Env *env,
                                     const std::string &modelPath,
                                     GraphDef *graph,
                                     const std::string &key256) {
        std::array<uint8_t, DEFAULT_KEY_SIZE> hashKey;
        
        picosha2::hash256_bytes(key256, hashKey);
        return GraphDefDecrypt(env, modelPath,
                               graph, hashKey,
                               internal::decryptAES);
    }
    
 
    template<typename In>
    inline void internal::decryptAES(const std::array<uint8_t, 32> &keyByteArray,
                                     std::vector<In> &input_content,
                                     const uint32_t content_size) {
        
        struct AES_ctx ctx;
        const std::vector<uint8_t> iv_bytes(input_content.begin(),
                                            input_content.begin() + AES_INIT_VECTOR_SIZE);
        
        AES_init_ctx_iv(&ctx, keyByteArray.data(), iv_bytes.data());
        
        input_content.erase(input_content.begin(),
                            input_content.begin() + AES_INIT_VECTOR_SIZE);
        
        AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(input_content.data()), content_size-AES_INIT_VECTOR_SIZE);
        
        const size_t size = input_content.size();
        const int last_index = (int)input_content[size - 1];
        size_t size_without_padding = size - last_index;
        input_content.resize(size_without_padding);
    }
}
