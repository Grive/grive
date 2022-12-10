FROM alpine:3.7 as build

RUN apk add make cmake g++ libgcrypt-dev yajl-dev yajl \
	boost-dev curl-dev expat-dev cppunit-dev binutils-dev \
	pkgconfig

ADD . /grive2

RUN mkdir /grive2/build \
	&& cd /grive2/build \
	&& cmake .. \
	&& make -j4 install

FROM alpine:3.7

RUN apk add yajl libcurl libgcrypt boost-program_options boost-regex libstdc++ boost-system \
	&& apk add boost-filesystem --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main

COPY --from=build /usr/local/bin/grive /bin/grive
RUN chmod 777 /bin/grive \
	&& mkdir /data

VOLUME /data
WORKDIR /data
ENTRYPOINT grive
