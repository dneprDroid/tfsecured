# Golang Example (MNIST)

## Install Tensorflow C lib

```bash 
export FILENAME=libtensorflow-cpu-linux-x86_64-1.5.0.tar.gz

wget -q --no-check-certificate https://storage.googleapis.com/tensorflow/libtensorflow/${FILENAME}

sudo tar -C /usr/local -xzf ${FILENAME}

sudo ldconfig /usr/local/lib
```

## Build and test

```bash 

go mod download

go build && \
    ./example \
    -image path/to/digit/image \
    -model ../../python/demo/models/saved_model-encrypted.pb \
    -key BXKE0351PD9TXZ7XA8CK8XZU8XBGDM
```