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

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/sha.h>

using namespace tensorflow;

namespace tfsecured {
    
    #define CHECK_AES_STATUS(status, ctx, msg) if (status == 0) {                   \
                                                EVP_CIPHER_CTX_cleanup(ctx);        \
                                                EVP_CIPHER_CTX_free(ctx);           \
                                                return errors::DataLoss(msg);       \
                                               }
    #define DEFAULT_KEY_SIZE       SHA256_DIGEST_LENGTH
    
    typedef std::array<uint8_t, DEFAULT_KEY_SIZE> KeyBytes;

    
    static const int32_t AES_INIT_VECTOR_SIZE = AES_BLOCK_SIZE;
    
    namespace internal  {
        inline Status decryptAES(const KeyBytes &key_bytes,
                                 std::vector<uint8_t> &input_content,
                                 const uint32_t content_size);
    }
    
    typedef std::function<Status(const KeyBytes &key_bytes,
                                 std::vector<uint8_t> &input_content,
                                 const uint32_t content_size)> Decryptor;
    
    inline Status GraphDefDecrypt(const std::string &modelPath,
                                  GraphDef *graph,
                                  const KeyBytes &keyByteArray,
                                  const Decryptor decryptor) {
        
        std::ifstream file(modelPath, std::ios::binary | std::ios::ate);
        std::vector<uint8_t> bytes;
        if (!file.eof() && !file.fail()) {
            file.seekg(0, std::ios_base::end);
            std::streampos fileSize = file.tellg();
            bytes.resize(fileSize);
            
            file.seekg(0, std::ios_base::beg);
            file.read((char*)&bytes[0], fileSize);
        } else {
            return errors::DataLoss("Can't read file at ", modelPath);
        }

        auto status = decryptor(keyByteArray, bytes, (uint32_t)file.tellg());
        if (!status.ok()) {
            return status;
        }
        if (!graph->ParseFromArray(bytes.data(), (int)bytes.size())) {
#ifdef DEBUG
            std::cout << "Invalid data: "
                      << std::string(bytes.begin(), bytes.end())
                      << std::endl;
            std::cout << "----------\nend invalid data block" << std::endl;;
#endif
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
        
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, key256.c_str(), key256.size());
        SHA256_Final(hashKey.data(), &sha256);
        
        return GraphDefDecrypt(modelPath,
                               graph, hashKey,
                               internal::decryptAES);
    }
 
    inline Status internal::decryptAES(const KeyBytes &key_bytes,
                                       std::vector<uint8_t> &input_content,
                                       const uint32_t content_size) {
        
        /*** Initialization ***/
        
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        
        int status = 1;
        
        const std::vector<uint8_t> iv_bytes(input_content.begin(),
                                            input_content.begin() + AES_INIT_VECTOR_SIZE);

        EVP_CIPHER_CTX_init(ctx);
        status = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key_bytes.data(), iv_bytes.data());
        CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptInit_ex Error");
        
        EVP_CIPHER_CTX_set_key_length(ctx, EVP_MAX_KEY_LENGTH);
        
        /*** Decryption ***/
        
        int p_len = (int)input_content.size();
        int f_len = 0;
        std::vector<unsigned char> result_buffer(p_len);
        
        status = EVP_DecryptUpdate(ctx,
                                   result_buffer.data(), &p_len,
                                   input_content.data() + AES_INIT_VECTOR_SIZE,
                                   (int) input_content.size() - AES_INIT_VECTOR_SIZE);
        CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptUpdate Error");

        status = EVP_DecryptFinal_ex(ctx, result_buffer.data() + p_len, &f_len);
        CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptFinal_ex Error");
        
        EVP_CIPHER_CTX_cleanup(ctx);
        EVP_CIPHER_CTX_free(ctx);
        
        int result_len = p_len + f_len;
        result_buffer.resize(result_len);
        
        input_content = result_buffer;
        
        return Status::OK();
    }
}
