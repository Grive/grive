project(libgrive)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenSSL REQUIRED)
find_package(JSONC REQUIRED)
find_package(CURL REQUIRED)
find_package(EXPAT REQUIRED)
find_package(CppUnit)

IF ( CPPUNIT_FOUND )
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
ENDIF ( CPPUNIT_FOUND )

include_directories(
	${libgrive_SOURCE_DIR}/src
	${OPT_INCS}
)

file(GLOB DRIVE_HEADERS
    ${libgrive_SOURCE_DIR}/src/drive/*.hh
)

file (GLOB PROTOCOL_HEADERS
    ${libgrive_SOURCE_DIR}/src/protocol/*.hh
)

file (GLOB UTIL_HEADERS
    ${libgrive_SOURCE_DIR}/src/util/*.hh
)

file (GLOB XML_HEADERS
    ${libgrive_SOURCE_DIR}/src/xml/*.hh
)

file (GLOB LIBGRIVE_SRC
	src/drive/*.cc
	src/http/*.cc
	src/protocol/*.cc
	src/util/*.cc
	src/xml/*.cc
)

add_definitions( -DVERSION="${GRIVE_VERSION}" )

add_library( grive SHARED ${LIBGRIVE_SRC} )

target_link_libraries( grive
	${CURL_LIBRARIES}
	${JSONC_LIBRARY}
	${OPENSSL_LIBRARIES}
	expat
)

set_target_properties(grive PROPERTIES
    SOVERSION 0 VERSION ${GRIVE_VERSION}
)

set ( LIBGRIVE_INSTALL_DIR lib)
if (EXISTS "${CMAKE_INSTALL_PREFIX}/lib32/" AND CMAKE_SIZEOF_VOID_P EQUAL 4)
	set (LIBGRIVE_INSTALL_DIR lib32)
elsif (EXISTS "${CMAKE_INSTALL_PREFIX}/lib64/" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	set (LIBGRIVE_INSTALL_DIR lib64)
endif ()

install(TARGETS grive LIBRARY DESTINATION ${LIBGRIVE_INSTALL_DIR} )
install(FILES ${DRIVE_HEADERS} DESTINATION include/grive/drive)
install(FILES ${PROTOCOL_HEADERS} DESTINATION include/grive/protocol)
install(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)
install(FILES ${XML_HEADERS} DESTINATION include/grive/xml)

IF ( CPPUNIT_FOUND )
	message("-- Building unitary tests along with the library and the binary")
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
    
    # list of test source files here
	file(GLOB TEST_SRC
		test/util/*.cc
		test/xml/*.cc
	)

	add_executable( unittest
		test/UnitTest.cc
		${TEST_SRC}
	)

	target_link_libraries( unittest
		grive
		${CPPUNIT_LIBRARY}
	)
        
ENDIF ( CPPUNIT_FOUND )
