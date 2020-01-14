# docker build . -t tfsecured-example:latest -f examples/cpp/Dockerfile

FROM floopcz/tensorflow_cc:ubuntu-2.2.0 AS builder 

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update &&   \
    apt-get install -y libssl-dev libprotobuf-dev libopencv-dev

WORKDIR app

COPY tfsecured/cpp tfsecured/cpp

COPY examples/cpp/CMakeLists.txt examples/cpp/
COPY examples/cpp/src/*.cpp examples/cpp/src/
COPY examples/cpp/src/*.h examples/cpp/src/

RUN cmake examples/cpp/ && make -j 5

ENTRYPOINT ["/app/example"]