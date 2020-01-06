#ifndef GraphDefDecryptor_Internals__hpp
#define GraphDefDecryptor_Internals__hpp

#include <iostream>
#include <fstream>

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/sha.h>

using namespace tensorflow;

namespace tfsecured {


    #define CHECK_AES_STATUS(status, ctx, msg) if (status == 0) {                   \
                                                ERR_print_errors_fp(stderr);        \
                                                EVP_CIPHER_CTX_cleanup(ctx);        \
                                                EVP_CIPHER_CTX_free(ctx);           \
                                                return errors::DataLoss(msg);       \
                                              }

    typedef std::array<uint8_t, SHA256_DIGEST_LENGTH> KeyBytes;


    static const int32_t AES_INIT_VECTOR_SIZE = AES_BLOCK_SIZE;

    namespace internal {
        
        
        inline void calculateSha256(const char *key, const size_t keyLen,
                                    unsigned char *out) {
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, key, keyLen);
            SHA256_Final(out, &sha256);
        }
        
        inline Status decryptAES(const KeyBytes &keyBytes,
                                 std::vector<uint8_t> &inputContent,
                                 const uint32_t contentSize) {
            
            std::cout << "OpenSSL version: " << OPENSSL_VERSION_TEXT << std::endl;
            if (inputContent.size() <= AES_INIT_VECTOR_SIZE) {
                return errors::InvalidArgument("Input encrypted content size = ",
                                               inputContent.size(),
                                               " is too small for AES CBC decryption.");
            }
            
            /*** Initialization ***/
            
            EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
            
            int status = 1;
            
            const std::vector<uint8_t> ivBytes(inputContent.begin(),
                                               inputContent.begin() + AES_INIT_VECTOR_SIZE);
            
            EVP_CIPHER_CTX_init(ctx);
            status = EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                                        keyBytes.data(), ivBytes.data());
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptInit_ex Error");
            
            /*** Decryption ***/
            
            int pLen = (int)inputContent.size();
            int fLen = 0;
            std::vector<uint8_t> resultBuffer(pLen);
            
            status = EVP_DecryptUpdate(ctx,
                                       resultBuffer.data(), &pLen,
                                       inputContent.data() + AES_INIT_VECTOR_SIZE,
                                       (int) inputContent.size() - AES_INIT_VECTOR_SIZE);
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptUpdate Error");
            
            status = EVP_DecryptFinal_ex(ctx, resultBuffer.data() + pLen, &fLen);
            CHECK_AES_STATUS(status, ctx, "[OpenSSL] EVP_DecryptFinal_ex Error");
            
            /*** Cleanup ***/
            
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);
            
            int resultLen = pLen + fLen;
            resultBuffer.resize(resultLen);
            
            inputContent = resultBuffer;
            
            return Status::OK();
        }
    }

}

#endif /* GraphDefDecryptor_Internals__hpp */
