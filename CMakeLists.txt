project(grive)

cmake_minimum_required(VERSION 2.8)

include(FindOpenSSL)

include_directories(
	${grive_SOURCE_DIR}/src
)

file(GLOB DRIVE_HEADERS
    ${grive_SOURCE_DIR}/src/drive/*.hh
)

file (GLOB PROTOCOL_HEADERS
    ${grive_SOURCE_DIR}/src/protocol/*.hh
)

file (GLOB UTIL_HEADERS
    ${grive_SOURCE_DIR}/src/util/*.hh
)

add_library( grive SHARED
	src/main.cc
	src/drive/Collection.cc
	src/drive/Drive.cc
	src/protocol/Download.cc
	src/protocol/HTTP.cc
	src/protocol/Json.cc
	src/protocol/OAuth2.cc
	src/util/Crypt.cc
	src/util/DateTime.cc
	src/util/OS.cc
)

add_executable( grive_cli
	src/main.cc
)

target_link_libraries( grive
	curl
	json
	${OPENSSL_LIBRARIES}
)

target_link_libraries( grive_cli
    grive
)

set_target_properties(grive PROPERTIES
    SOVERSION 0 VERSION 0.0.1
)

add_executable( unittest
	test/UnitTest.cc
	src/util/DateTime.cc
	test/util/DateTimeTest.cc
	test/util/FunctionTest.cc
)

target_link_libraries( unittest
	cppunit
)


## Install targets
install(TARGETS grive LIBRARY DESTINATION lib)
install(TARGETS grive_cli RUNTIME DESTINATION bin)
install(FILES ${DRIVE_HEADERS} DESTINATION include/grive/drive)
install(FILES ${PROTOCOL_HEADERS} DESTINATION include/grive/protocol)
install(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)
