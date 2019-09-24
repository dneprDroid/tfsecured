## TFSecured

Small library for tensorflow proto models (*.pb) encryption/decryption.

## AES

You may use random string with random length like a key, then library calculates sha256 hash of it and uses as internal key with size 256 bits.

## Usage 

Copy sources from [TFSecured](/TFSecured) dir into your project.
 
C++ usage (see [TFPredictor.mm](/iosDemo/TFSecured-iOS/tf/predict/TFPredictor.mm)):

```cpp

    #include <GraphDefDecryptor.hpp>

    ........


    tensorflow::GraphDef graph;
    // Decryption: 
    const std::string key = "JHEW8F7FE6F8E76W8F687WE6F8W8EF5";
    auto status = tfsecured::GraphDefDecryptAES(path,         // path to *.pb file (frozen graph)
                                                &graph,
                                                key);         // your key
    if (!status.ok()) {
        std::cout << status.error_message() << std::endl;
        return;
    }
    
    // Create session :
    std::unique_ptr<Session> session(NewSession(options));
    status = session->Create(graph);
    
    // Run session ....
```


Encrypt model via python script (see [encrypt_model.py](/python/encrypt_model.py)):

```bash
$ python encrypt_model.py <INPUT_PB_MODEL>  \
                          <OUTPUT_PB_MODEL> \  
                          <KEY>                # optional, generated randomly by script 

```

## Dependencies

* TensorFlow

* OpenSSL

## iOS Demo (Digit recognizer)

Prepare repository (install Tensorflow via pods)

```bash

$ cd iosDemo
$ pod install 

``` 
And open it in Xcode.
