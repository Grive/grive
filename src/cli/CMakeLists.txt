
add_executable( grive_executable
	main.cc
)

target_link_libraries( grive_executable
    grive 
)

set_target_properties( grive_executable
    PROPERTIES OUTPUT_NAME grive
)

install(TARGETS grive_executable RUNTIME DESTINATION bin)