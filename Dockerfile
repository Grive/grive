FROM ubuntu:16.04

RUN apt-get update && \
  apt-get install --yes --no-install-recommends g++ cmake build-essential \
  libgcrypt11-dev libyajl-dev libboost-all-dev \
  libcurl4-openssl-dev libexpat1-dev libcppunit-dev \
  binutils-dev pkg-config zlib1g-dev && \
  rm -rf /var/cache/apt/archives /var/lib/apt/lists/*

ADD https://github.com/Yelp/dumb-init/releases/download/v1.2.1/dumb-init_1.2.1_amd64 /usr/local/bin/dumb-init

RUN chmod +x /usr/local/bin/dumb-init

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

ENTRYPOINT ["dumb-init", "/usr/local/bin/grive"]


# docker build -t grive2 .
# docker run --name grive2 --rm -it -v $PWD:/data grive2:latest