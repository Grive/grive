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

nstall(FILES ${UTIL_HEADERS} DESTINATION include/grive/util)