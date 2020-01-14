# C++ Example

## Build with Docker

```bash 
docker build . -t tfsecured-example:latest -f examples/cpp/Dockerfile
```

## Test

```bash 

docker run \
    -v $(realpath ./python/demo/models):/models \
    -v $(realpath ~/Downloads):/image_downloads \
    tfsecured-example:latest \
    \
    /models/saved_model-encrypted.pb \
    /image_downloads/some_digit.JPEG \
    BXKE0351PD9TXZ7XA8CK8XZU8XBGDM

```