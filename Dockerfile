FROM ubuntu:16.04

RUN apt-get update && \
  apt-get install --yes git cmake build-essential \
  libgcrypt11-dev libyajl-dev libboost-all-dev \
  libcurl4-openssl-dev libexpat1-dev libcppunit-dev \
  binutils-dev pkg-config zlib1g-dev && \
  rm -rf /var/cache/apt/archives

VOLUME /data

WORKDIR /data



# docker build -t grive2 .
# docker run --name grive2 --rm -it -v $PWD:/data grive2:latest