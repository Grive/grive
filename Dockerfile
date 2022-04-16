FROM alpine:3.7 as build

RUN apk add git make cmake g++ libgcrypt-dev yajl-dev yajl \
	boost-dev curl-dev expat-dev cppunit-dev binutils-dev \
	pkgconfig \
	&& git clone https://github.com/vitalif/grive2.git \
	&& mkdir grive2/build \
	&& cd grive2/build  \
	&& cmake ..  \
	&& make -j4  \
	&& make install \
	&& cd ../.. \
	&& rm -rf grive2 \
	&& mkdir /drive

FROM alpine:3.7
COPY --from=build /usr/local/bin/grive /bin/grive
ADD https://github.com/Yelp/dumb-init/releases/download/v1.2.1/dumb-init_1.2.1_amd64 /bin/dumb-init
RUN chmod 777 /bin/dumb-init /bin/grive \
	&& mkdir /data \
	&& apk add yajl libcurl libgcrypt \
	boost-program_options boost-regex libstdc++ boost-system boost binutils \
	&& apk add boost-filesystem --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main

VOLUME /data
WORKDIR /data
ENTRYPOINT ["dumb-init", "grive"]
