project(grive)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenSSL REQUIRED)
find_package(JSONC REQUIRED)
find_package(CURL REQUIRED)
find_package(CppUnit)

IF ( CPPUNIT_FOUND )
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
ENDIF ( CPPUNIT_FOUND )

include_directories(
	${grive_SOURCE_DIR}/src
	${OPT_INCS}
)

add_executable( grive
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

target_link_libraries( grive
	${CURL_LIBRARIES}
	${JSONC_LIBRARY}
	${OPENSSL_LIBRARIES}
)

IF ( CPPUNIT_FOUND )

	add_executable( unittest
		test/UnitTest.cc
		src/util/DateTime.cc
		test/util/DateTimeTest.cc
		test/util/FunctionTest.cc
	)

	target_link_libraries( unittest
		${CPPUNIT_LIBRARY}
	)

ELSE ( CPPUNIT_FOUND )
	message( STATUS "skip building unittest" )
ENDIF ( CPPUNIT_FOUND )

INSTALL ( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/grive DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
