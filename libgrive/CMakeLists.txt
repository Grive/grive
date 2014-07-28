project(libgrive)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LibGcrypt REQUIRED)
find_package(JSONC REQUIRED)
find_package(CURL REQUIRED)
find_package(EXPAT REQUIRED)
find_package(YAJL REQUIRED)
find_package(Boost 1.40.0 COMPONENTS program_options filesystem unit_test_framework system REQUIRED)
find_package(BFD)
find_package(CppUnit)
find_package(Iberty)
find_package(ZLIB)

# additional headers if build unit tests
IF ( CPPUNIT_FOUND )
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
ENDIF ( CPPUNIT_FOUND )

# build bfd classes if libbfd is found
if ( BFD_FOUND )
	set( OPT_LIBS	${DL_LIBRARY} ${BFD_LIBRARY} )
	file( GLOB OPT_SRC
		src/bfd/*.cc
	)
	add_definitions( -DHAVE_BFD )
	
endif ( BFD_FOUND )

if ( IBERTY_FOUND )
	set( OPT_LIBS	${OPT_LIBS}	${IBERTY_LIBRARY} )
else ( IBERTY_FOUND )
  set( IBERTY_LIBRARY "" )
endif ( IBERTY_FOUND )

if ( ZLIB_FOUND )
	set( OPT_LIBS	${OPT_LIBS}	${ZLIB_LIBRARIES} )
endif ( ZLIB_FOUND )

include_directories(
	${libgrive_SOURCE_DIR}/src
	${libgrive_SOURCE_DIR}/test
	${GDBM_INCLUDE_DIR}
	${YAJL_INCLUDE_DIR}
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
	src/drive2/*.cc
	src/http/*.cc
	src/protocol/*.cc
	src/json/*.cc
	src/util/*.cc
	src/util/log/*.cc
	src/xml/*.cc
)

add_definitions(
	-DTEST_DATA="${libgrive_SOURCE_DIR}/test/data/"
	-DSRC_DIR="${libgrive_SOURCE_DIR}/src"
)

#add_library( grive SHARED ${LIBGRIVE_SRC} ${OPT_SRC} )
add_library( grive STATIC ${LIBGRIVE_SRC} ${OPT_SRC} )

target_link_libraries( grive
	${YAJL_LIBRARY}
	${CURL_LIBRARIES}
	${JSONC_LIBRARY}
	${LIBGCRYPT_LIBRARIES}
	${GDBM_LIBRARIES}
	${Boost_LIBRARIES}
	${IBERTY_LIBRARY}
	${EXPAT_LIBRARY}
	${OPT_LIBS}
)

#set_target_properties(grive PROPERTIES
#    SOVERSION 0 VERSION ${GRIVE_VERSION}
#)

#set_target_properties(grive grive-static PROPERTIES OUTPUT_NAME grive)

if ( LIB_INSTALL_DIR )
else()
	set ( LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib )
endif()

# don't install libgrive for now
#install(TARGETS grive LIBRARY DESTINATION ${LIB_INSTALL_DIR} )
#install(TARGETS grive ARCHIVE DESTINATION ${LIB_INSTALL_DIR} )
#install(FILES ${DRIVE_HEADERS} DESTINATION include/grive/drive)
#install(FILES ${PROTOCOL_HEADERS} DESTINATION include/grive/protocol)
#install(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)
#install(FILES ${XML_HEADERS} DESTINATION include/grive/xml)

IF ( CPPUNIT_FOUND )
	message("-- Building unitary tests along with the library and the binary")
	set( OPT_INCS		${CPPUNIT_INCLUDE_DIR} )
    
    # list of test source files here
	file(GLOB TEST_SRC
		test/drive/*.cc
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
		${Boost_LIBRARIES}
	)
        
ENDIF ( CPPUNIT_FOUND )

file(GLOB BTEST_SRC
	test/btest/*.cc
)

add_executable( btest ${BTEST_SRC} )

target_link_libraries( btest
	grive
	${Boost_LIBRARIES}
)

if ( WIN32 )
else ( WIN32 )
	set_target_properties( btest
		PROPERTIES COMPILE_FLAGS	 -DBOOST_TEST_DYN_LINK )
endif (WIN32)
