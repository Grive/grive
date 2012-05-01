project(grive)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenSSL REQUIRED)
find_package(JSONC REQUIRED)
find_package(CURL REQUIRED)
find_package(CppUnit)

# Common include
include_directories( ${grive_SOURCE_DIR}/src/lib
    ${OPT_INCS}
)

add_subdirectory(src)

IF ( CPPUNIT_FOUND )
    message("-- Building unitary tests along with the library and the binary")
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
    add_subdirectory(test)
ENDIF ( CPPUNIT_FOUND )

