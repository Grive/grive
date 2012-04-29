project(grive)

cmake_minimum_required(VERSION 2.8)

include(FindOpenSSL)

###############################################################################
# finding cppunit
###############################################################################
find_path( CPPUNIT_INCLUDE_DIR cppunit/TestFixture.h /usr/include
                                                     /usr/local/include
                                                     ${CPPUNIT_PREFIX}/include )
find_library( CPPUNIT_LIBRARY_DEBUG NAMES cppunit cppunit_dll
              PATHS /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64
                    ${CPPUNIT_PREFIX}/lib
              PATH_SUFFIXES debug )

find_library( CPPUNIT_LIBRARY_RELEASE NAMES cppunit cppunit_dll
              PATHS /usr/lib
                    /usr/lib64
                    /usr/local/lib
                    /usr/local/lib64
                    ${CPPUNIT_PREFIX}/lib
              PATH_SUFFIXES release )

set( CPPUNIT_LIBRARY debug     ${CPPUNIT_LIBRARY_DEBUG}
                     optimized ${CPPUNIT_LIBRARY_RELEASE} )

if ( CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY )
	message( STATUS "found cppunit" )
	set( CPPUNIT_FOUND	TRUE )
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
endif ( CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY )

###############################################################################

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
	curl
	json
	${OPENSSL_LIBRARIES}
)

if ( CPPUNIT_FOUND )

add_executable( unittest
	test/UnitTest.cc
	src/util/DateTime.cc
	test/util/DateTimeTest.cc
	test/util/FunctionTest.cc
)

target_link_libraries( unittest
	${CPPUNIT_LIBRARY}
)

else ( CPPUNIT_FOUND )
	message( STATUS "skip building unittest" )
endif ( CPPUNIT_FOUND )
