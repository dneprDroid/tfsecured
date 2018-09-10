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
#include "internal/picosha2.hpp"
#include "internal/aes.hpp"

using namespace tensorflow;

namespace tfsecured {

    #define DEFAULT_KEY_SIZE 32
    
    typedef std::array<uint8_t, DEFAULT_KEY_SIZE> KeyBytes;

    static const int32_t AES_BLOCK_SIZE       = AES_BLOCKLEN;
    static const int32_t AES_INIT_VECTOR_SIZE = AES_BLOCK_SIZE;
    
    namespace internal  {
        template<typename In>
        inline Status decryptAES(const KeyBytes &key_bytes,
                                 std::vector<In> &input_content,
                                 const uint32_t content_size);
    }
    
    typedef Status (*Decryptor)(const KeyBytes &key_bytes,
                                std::vector<char> &input_content,
                                const uint32_t content_size);
    
    inline Status GraphDefDecrypt(const std::string &modelPath,
                                  GraphDef *graph,
                                  const KeyBytes &keyByteArray,
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

        auto status = decryptor(keyByteArray, bytes, (uint32_t)file.tellg());
        if (!status.ok()) {
            return status;
        }
        if (!graph->ParseFromArray(bytes.data(), (int)bytes.size())) {
            return errors::DataLoss("Can't parse ", modelPath, " as binary proto");
        }
        return Status::OK();
    }
    
    inline Status GraphDefDecrypt(const std::string &modelPath,
                                  GraphDef *graph,
                                  const KeyBytes &keyByteArray) {
        return GraphDefDecrypt(modelPath,
                               graph, keyByteArray,
                               internal::decryptAES);
    }
    
    inline Status GraphDefDecryptAES(const std::string &modelPath,
                                     GraphDef *graph,
                                     const std::string &key256) {
        KeyBytes hashKey;
        
        picosha2::hash256_bytes(key256, hashKey);
        return GraphDefDecrypt(modelPath,
                               graph, hashKey,
                               internal::decryptAES);
    }
 
    template<typename In>
    inline Status internal::decryptAES(const KeyBytes &key_bytes,
                                       std::vector<In> &input_content,
                                       const uint32_t content_size) {
        if (input_content.size() < AES_INIT_VECTOR_SIZE) {
            return errors::InvalidArgument("Input encrypted content size = ",
                                           input_content.size(),
                                           " is too small for AES CBC decryption.");
        }
        struct AES_ctx ctx;
        const std::vector<uint8_t> iv_bytes(input_content.begin(),
                                            input_content.begin() + AES_INIT_VECTOR_SIZE);
        
        AES_init_ctx_iv(&ctx, key_bytes.data(), iv_bytes.data());
        
        input_content.erase(input_content.begin(),
                            input_content.begin() + AES_INIT_VECTOR_SIZE);
        
        AES_CBC_decrypt_buffer(&ctx, reinterpret_cast<uint8_t*>(input_content.data()), content_size-AES_INIT_VECTOR_SIZE);
        
        const size_t size = input_content.size();
        const int last_index = (int)input_content[size - 1];
        if (last_index < 0 || last_index >= size) {
            return errors::InvalidArgument("Decryption failed for this key.");
        }
        size_t size_without_padding = size - last_index;
        input_content.resize(size_without_padding);
        return Status::OK();
    }
}
