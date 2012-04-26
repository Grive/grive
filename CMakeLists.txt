project(grive)

cmake_minimum_required(VERSION 2.8)

include(FindOpenSSL)

add_executable( grive
	src/main.cc
	src/OAuth2.cc
	src/Drive.cc
	src/Collection.cc
	src/protocol/HTTP.cc
	src/protocol/Json.cc
	src/protocol/Download.cc )

target_link_libraries( grive
	curl
	json
	${OPENSSL_LIBRARIES} )
