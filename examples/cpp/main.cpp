#include <iostream>
#include <memory>
#include <vector>

#include <GraphDefDecryptor.hpp>

using namespace std;

void printError(const tensorflow::Status &status, const std::string &msg = "") {
    string errMsg = msg.empty() ? "Got error: " : (msg + " error: ");
    std::cout << errMsg << status.error_message() << std::endl;
}

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
    tensorflow::Tensor input; // TODO: image to tensor
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
    return 0;
}