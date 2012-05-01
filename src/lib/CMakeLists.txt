# lib subproject

file(GLOB DRIVE_SOURCES
    drive/*.cc
)

file(GLOB PROTOCOL_SOURCES
    protocol/*.cc
)

file(GLOB UTIL_SOURCES
    util/*.cc
)

file(GLOB DRIVE_HEADERS
    drive/*.hh
)

file (GLOB PROTOCOL_HEADERS
    protocol/*.hh
)

file (GLOB UTIL_HEADERS
    util/*.hh
)

add_library( grive SHARED
    ${DRIVE_SOURCES}
    ${PROTOCOL_SOURCES}
    ${UTIL_SOURCES}
)

target_link_libraries( grive
	${CURL_LIBRARIES}
	${JSONC_LIBRARY}
	${OPENSSL_LIBRARIES}
)

set_target_properties(grive PROPERTIES
    SOVERSION 0 VERSION 0.0.1
)

install(TARGETS grive LIBRARY DESTINATION lib)
install(FILES ${DRIVE_HEADERS} DESTINATION include/grive/drive)
install(FILES ${PROTOCOL_HEADERS} DESTINATION include/grive/protocol)
install(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)