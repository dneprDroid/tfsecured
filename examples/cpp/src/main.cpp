#include <iostream>
#include <memory>
#include <vector>

#include <GraphDefDecryptor.hpp>

#include "img2tensor.h"

using namespace std;
using namespace tensorflow;

void printError(const Status &status, const string &msg = "");

Tensor loadImgTensor(const string &path, int w, int h);

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

    GraphDef graph;

    auto status = tfsecured::GraphDefDecryptAES(pbPath, graph, key);
    if (!status.ok()) {
        printError(status, "GraphDefDecryptAES");
        return 1;
    }
    std::unique_ptr<Session> session(NewSession({}));
    status = session->Create(graph);
    if (!status.ok()) {
        printError(status, "Session create");
        return 1;
    }
    Tensor input = loadImgTensor(imgPath, 28, 28);
    vector<Tensor> outputs;

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
    auto output = outputs[0].flat<float>();
    
    for (int i=0; i < output.size(); ++i) {
        float prob = output(i);
        std::cout << "Probability for digit " << (i + 1) << " = " << prob << "\n";
    }
    return 0;
}

void printError(const Status &status, const string &msg) {
    string errMsg = msg.empty() ? "Got error: " : (msg + " error: ");
    std::cout << errMsg << status.error_message() << "\n";
}

Tensor loadImgTensor(const string &path, int w, int h) {
    Tensor tensor(
        DT_FLOAT, 
        TensorShape({1, w * h})
    );
    float *p = tensor.flat<float>().data();
    fillTensor(path, w, h, p);
    return tensor;
}