project( grive )

include_directories(
	${grive_SOURCE_DIR}/../libgrive/src
	${OPT_INCS}
)

add_executable( grive_executable
	src/main.cc
)

target_link_libraries( grive_executable
    grive 
)

set_target_properties( grive_executable
    PROPERTIES OUTPUT_NAME grive
)

install(TARGETS grive_executable RUNTIME DESTINATION bin)
