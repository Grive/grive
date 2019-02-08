cmake_minimum_required(VERSION 2.8)

include(GNUInstallDirs)

# Grive version. remember to update it for every new release!
set( GRIVE_VERSION "0.5.1-dev" )

# common compile options
add_definitions( -DVERSION="${GRIVE_VERSION}" )
add_definitions( -D_FILE_OFFSET_BITS=64 -std=c++0x )
if ( APPLE )
	add_definitions( -Doff64_t=off_t )
endif ( APPLE )

find_program(
	HAVE_SYSTEMD systemd
	PATHS /lib/systemd /usr/lib/systemd
	NO_DEFAULT_PATH
)
if ( HAVE_SYSTEMD )
	add_subdirectory( systemd )
endif( NOT HAVE_SYSTEMD )

add_subdirectory( libgrive )
add_subdirectory( grive )

