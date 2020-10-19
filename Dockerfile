FROM gcc
RUN apt-get update
RUN apt-get install -y \
            build-essential \
            gdb \
            autoconf-archive \
            git \
            cmake \
            doxygen \
            valgrind \
            strace

# Install the testing framework
RUN cp /usr/src/graph/cmocka-1.1.0.tar.xz /usr/src/
RUN cd /usr/src/ && \
    mkdir build && \
    cd build && \
    cmake ../cmocka-1.1.0 && \
    make install && \
    export LD_LIBRARY_PATH=/usr/local/lib/

# Install json-c
RUN git clone https://github.com/json-c/json-c.git
RUN mkdir json-c-build
RUN cd json-c-build && \
    cmake ../json-c && \
    make && \
    make install

WORKDIR /usr/src/graph
ENV PATH="/usr/src/graph:${PATH}"
CMD ["bash"]
