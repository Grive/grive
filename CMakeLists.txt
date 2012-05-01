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

file(GLOB DRIVE_HEADERS
    ${grive_SOURCE_DIR}/src/drive/*.hh
)

file (GLOB PROTOCOL_HEADERS
    ${grive_SOURCE_DIR}/src/protocol/*.hh
)

file (GLOB UTIL_HEADERS
    ${grive_SOURCE_DIR}/src/util/*.hh
)

add_library( fgrive SHARED
	src/drive/Collection.cc
	src/drive/Drive.cc
	src/drive/File.cc
	src/protocol/Download.cc
	src/protocol/HTTP.cc
	src/protocol/Json.cc
	src/protocol/OAuth2.cc
	src/util/Crypt.cc
	src/util/DateTime.cc
	src/util/OS.cc
	src/util/Path.cc
)

add_executable( grive
	src/main.cc
)

target_link_libraries( fgrive
	${CURL_LIBRARIES}
	${JSONC_LIBRARY}
	${OPENSSL_LIBRARIES}
)

target_link_libraries( grive
    fgrive
)

set_target_properties(fgrive PROPERTIES
    SOVERSION 0 VERSION 0.0.1
)

IF ( CPPUNIT_FOUND )
	add_executable( unittest
		test/UnitTest.cc
		test/util/DateTimeTest.cc
		test/util/FunctionTest.cc
		test/util/PathTest.cc
	)
		
    target_link_libraries( unittest
		fgrive
		${CPPUNIT_LIBRARY}
    )
    else ( CPPUNIT_FOUND )
	    message( STATUS "skip building unittest" )
endif ( CPPUNIT_FOUND )


## Install targets
install(TARGETS fgrive LIBRARY DESTINATION lib)
install(TARGETS grive RUNTIME DESTINATION bin)
install(FILES ${DRIVE_HEADERS} DESTINATION include/grive/drive)
install(FILES ${PROTOCOL_HEADERS} DESTINATION include/grive/protocol)
install(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)
