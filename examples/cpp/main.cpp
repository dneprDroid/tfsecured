#include <iostream>
#include <memory>
#include <vector>

#include <GraphDefDecryptor.hpp>

#include "img2tensor.h"

using namespace std;

void printError(const tensorflow::Status &status, const std::string &msg = "");

tensorflow::Tensor loadImgTensor(const std::string &path, int w, int h);

int main(int argc, const char * argv[]) {
    if (argc < 3) {
        std::cout 
            << "Invalid args, usage: "
            << "`./example path/to/encrypted/pb/file " 
            << " path/to/image someKey` "
            << std::endl;
        return 1;
    }
    
    const string pbPath = argv[1];
    const string imgPath = argv[2];
    const string key = argv[3];

    tensorflow::GraphDef graph;

    auto status = tfsecured::GraphDefDecryptAES(pbPath, graph, key);
    if (!status.ok()) {
        printError(status, "GraphDefDecryptAES");
        return 1;
    }
    SessionOptions options;
    std::unique_ptr<Session> session(NewSession(options));
    status = session->Create(graph);
    if (!status.ok()) {
        printError(status, "Session create");
        return 1;
    }
    tensorflow::Tensor input = loadImgTensor(imgPath, 29, 29);
    vector<tensorflow::Tensor> outputs;

    status = session->Run(
        {},
        {{"nn_input", input}},
        {"nn_output"},
        {},
        &outputs,
        nullptr
    );
    if (!status.ok()) {
        printError(status, "Session run");
        return 1;
    }
    auto output = outputs[0];
    auto flatTensor = output.flat<float>();
    int digit = -1;
    float digitProb = -1;
    
    for (int i=0; i < flatTensor.size(); ++i) {
        float prob = flatTensor(i);
        std::cout << "Probability for digit " << (i + 1) << " = " << prob << "\n";
        if (prob > digitProb) {
            digitProb = prob;
            digit = i + 1;
        }
    }
    std::cout << "Digit: " << digit << "\n";
    return 0;
}

void printError(const tensorflow::Status &status, const std::string &msg) {
    string errMsg = msg.empty() ? "Got error: " : (msg + " error: ");
    std::cout << errMsg << status.error_message() << std::endl;
}

tensorflow::Tensor loadImgTensor(const std::string &path, int w, int h) {
    tensorflow::Tensor tensor(
        tensorflow::DT_FLOAT, 
        tensorflow::TensorShape({1, w * h})
    );
    float *p = tensor.flat<float>().data();
    fillTensor(path, w, h, p);
    return tensor;
}