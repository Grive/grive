project(grive)

cmake_minimum_required(VERSION 2.8)

include(FindOpenSSL)

include_directories( ${grive_SOURCE_DIR}/src )

add_executable( grive
	src/main.cc
	src/drive/Collection.cc
	src/drive/Drive.cc
	src/protocol/Download.cc
	src/protocol/HTTP.cc
	src/protocol/Json.cc
	src/protocol/OAuth2.cc
	src/util/OS.cc
)

target_link_libraries( grive
	curl
	json
	${OPENSSL_LIBRARIES} )
