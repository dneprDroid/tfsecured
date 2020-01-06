#ifndef GraphDefDecryptor__hpp
#define GraphDefDecryptor__hpp


#include <iostream>
#include <fstream>

#include <tensorflow/core/framework/op.h>
#include <tensorflow/core/framework/op_kernel.h>
#include <tensorflow/core/public/session.h>

#include <tensorflow/core/framework/shape_inference.h>

#include "GraphDefDecryptor.Internals.hpp"

using namespace tensorflow;

namespace tfsecured {
    
    
    typedef std::function<tensorflow::Status(const KeyBytes &keyBytes,
                                             std::vector<uint8_t> &inputContent,
                                             const uint32_t content_size)> Decryptor;
    
    
    inline Status GraphDefDecrypt(const std::string &modelPath,
                                  GraphDef &graph,
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
        if (!graph.ParseFromArray(bytes.data(), (int)bytes.size())) {
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
                                  GraphDef &graph,
                                  const KeyBytes &keyByteArray) {
        return GraphDefDecrypt(modelPath,
                               graph, keyByteArray,
                               internal::decryptAES);
    }
    
    
    inline Status GraphDefDecryptAES(const std::string &modelPath,
                                     GraphDef &graph,
                                     const std::string &key256) {
        KeyBytes hashKey;
        
        internal::calculateSha256(key256.c_str(), key256.size(),
                                  hashKey.data());
        
        return GraphDefDecrypt(modelPath,
                               graph, hashKey,
                               internal::decryptAES);
    }
 
    
}

#endif /* GraphDefDecryptor__hpp */
