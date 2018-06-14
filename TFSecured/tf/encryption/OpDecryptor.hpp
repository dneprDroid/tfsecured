//
//  OpDecryptor.hpp
//  TFSecured
//
//  Created by user on 6/14/18.
//  Copyright © 2018 user. All rights reserved.
//

#include <stdio.h>

#include <string>
#include <array>
#include <tensorflow/core/public/session.h>


namespace tf_secured {
    

    void DecryptOpsNames(tensorflow::GraphDef &graph, const std::string &key256);
    void DecryptOpsNames(tensorflow::GraphDef &graph, const std::array<uint8_t, 32> &keyByteArray);

    
}
