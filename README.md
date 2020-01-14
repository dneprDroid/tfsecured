## tfsecured

Small library for tensorflow proto models (*.pb) encryption/decryption.

## AES

You may use random string with random length like a key, then library calculates sha256 hash of it and uses as internal key with size 256 bits.

## Usage 

### Golang 

```golang 
    import (
        tfsecured "github.com/dneprDroid/tfsecured/tfsecured/go"
    )

    ...

    decryptor, err := tfsecured.NewDecryptorFile(key, path)
	if err != nil {
		log.Fatalf("file open failed: %v", err)
	}
	graph, err := decryptor.Decrypt()
	if err != nil {
		log.Fatalf("decryption failed: %v", err)
	}
    // Create session :
    
	session, err := tf.NewSession(graph, nil)
	if err != nil {
		log.Fatalf("session failed: %v", err)
	}

    // Run session ....
```

### C++

Copy sources from [tfsecured](/tfsecured) dir into your project (see [CMakeLists.txt](/examples/cpp/CMakeLists.txt))
 
C++ usage (see example [main.cpp](/examples/cpp/main.cpp)):

```cpp

    #include <GraphDefDecryptor.hpp>

    ........


    tensorflow::GraphDef graph;
    // Decryption: 
    const std::string key = "JHEW8F7FE6F8E76W8F687WE6F8W8EF5";
    auto status = tfsecured::GraphDefDecryptAES(path,         // path to *.pb file (frozen graph)
                                               graph,
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

## Dependencies (C++)

* TensorFlow

* OpenSSL
