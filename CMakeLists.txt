cmake_minimum_required(VERSION 2.8)

# Grive version. remember to update it for every new release!
set( GRIVE_VERSION "0.2.0" )

# Actually find the boost libraries
find_package( Boost )

# common compile options
add_definitions( -DVERSION="${GRIVE_VERSION}" )
add_definitions( -D_FILE_OFFSET_BITS=64 )

add_subdirectory( libgrive )
add_subdirectory( grive )
