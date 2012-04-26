project(grive)

cmake_minimum_required(VERSION 2.8)

include(FindOpenSSL)

add_executable( grive
	src/main.cc
	src/OAuth2.cc
	src/HTTP.cc
	src/Json.cc
	src/Drive.cc
	src/Download.cc )

target_link_libraries( grive
	curl
	json
	${OPENSSL_LIBRARIES} )
