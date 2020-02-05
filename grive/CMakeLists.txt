project( grive )

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(
	${grive_SOURCE_DIR}/../libgrive/src
	${OPT_INCS}
	${Boost_INCLUDE_DIRS}
)

file (GLOB GRIVE_EXE_SRC
	${grive_SOURCE_DIR}/src/*.cc
)

add_executable( grive_executable
	${GRIVE_EXE_SRC}
)

target_link_libraries( grive_executable
	grive
)

set(DEFAULT_APP_ID "615557989097-i93d4d1ojpen0m0dso18ldr6orjkidgf.apps.googleusercontent.com")
set(DEFAULT_APP_SECRET "xiM8Apu_WuRRdheNelJcNtOD")
set(APP_ID ${DEFAULT_APP_ID} CACHE STRING "Application Id")
set(APP_SECRET ${DEFAULT_APP_SECRET} CACHE STRING "Application Secret")

target_compile_definitions ( grive_executable
	PRIVATE
		-DAPP_ID="${APP_ID}"
		-DAPP_SECRET="${APP_SECRET}"
)

set_target_properties( grive_executable
	PROPERTIES OUTPUT_NAME grive
)

install(TARGETS grive_executable RUNTIME DESTINATION bin)

if ( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" )
    install(FILES doc/grive.1 DESTINATION man/man1 )
else ( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" )
    install(FILES doc/grive.1 DESTINATION share/man/man1 )
endif( ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" )
