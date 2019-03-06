//
//  GraphDefDecryptorInternals.hpp
//  TFSecured-iOS
//
//  Created by user on 3/7/19.
//  Copyright © 2019 user. All rights reserved.
//


#include <stdio.h>

#include <iostream>
#include <fstream>

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>

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

    namespace internal {
        
        
        inline void calculateSha256(const char *key, const size_t key_len,
                                    unsigned char *out) {
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, key, key_len);
            SHA256_Final(out, &sha256);
        }
        
        inline Status decryptAES(const KeyBytes &key_bytes,
                                 std::vector<uint8_t> &input_content,
                                 const uint32_t content_size) {
            
            if (input_content.size() <= AES_INIT_VECTOR_SIZE) {
                return errors::InvalidArgument("Input encrypted content size = ",
                                               input_content.size(),
                                               " is too small for AES CBC decryption.");
            }
            
            /*** Initialization ***/
            
            EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
            
            int status = 1;
            
            const std::vector<uint8_t> iv_bytes(input_content.begin(),
                                                input_content.begin() + AES_INIT_VECTOR_SIZE);
            
            EVP_CIPHER_CTX_init(ctx);
            status = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                                        key_bytes.data(), iv_bytes.data());
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptInit_ex Error");
            
            EVP_CIPHER_CTX_set_key_length(ctx, EVP_MAX_KEY_LENGTH);
            
            /*** Decryption ***/
            
            int p_len = (int)input_content.size();
            int f_len = 0;
            std::vector<uint8_t> result_buffer(p_len);
            
            status = EVP_DecryptUpdate(ctx,
                                       result_buffer.data(), &p_len,
                                       input_content.data() + AES_INIT_VECTOR_SIZE,
                                       (int) input_content.size() - AES_INIT_VECTOR_SIZE);
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptUpdate Error");
            
            status = EVP_DecryptFinal_ex(ctx, result_buffer.data() + p_len, &f_len);
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptFinal_ex Error");
            
            /*** Cleanup ***/
            
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);
            
            int result_len = p_len + f_len;
            result_buffer.resize(result_len);
            
            input_content = result_buffer;
            
            return Status::OK();
        }
    }

}
