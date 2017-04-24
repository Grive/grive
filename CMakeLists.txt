cmake_minimum_required(VERSION 2.8)

include(GNUInstallDirs)

# Grive version. remember to update it for every new release!
set( GRIVE_VERSION "0.5.1-dev" )

# common compile options
add_definitions( -DVERSION="${GRIVE_VERSION}" )
add_definitions( -D_FILE_OFFSET_BITS=64 -std=c++0x )

add_subdirectory( systemd )
add_subdirectory( libgrive )
add_subdirectory( grive )
	