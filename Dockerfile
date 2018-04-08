FROM ubuntu:16.04

RUN apt-get update && \
  apt-get install --yes git cmake build-essential \
  libgcrypt11-dev libyajl-dev libboost-all-dev \
  libcurl4-openssl-dev libexpat1-dev libcppunit-dev \
  binutils-dev pkg-config zlib1g-dev && \
  rm -rf /var/cache/apt/archives

ADD . /grive2

RUN cd /grive2 && \
  mkdir build && \
  cd build && \
  cmake .. && \
  make -j4 && \
  mv /grive2/build/grive/grive /usr/local/bin/grive && \
  rm -rf /grive2

VOLUME /data

WORKDIR /data

ENTRYPOINT ["/usr/local/bin/grive"]


# docker build -t grive2 .
# docker run --name grive2 --rm -it -v $PWD:/data grive2:latest