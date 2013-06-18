cmake_minimum_required(VERSION 2.8)

# Grive version. remember to update it for every new release!
set( GRIVE_VERSION "0.3.0-pre" )

# common compile options
add_definitions( -DVERSION="${GRIVE_VERSION}" )
add_definitions( -D_FILE_OFFSET_BITS=64 )

# Package name and version for libraries depending on autotools
add_definitions( -DPACKAGE="grive" )
add_definitions( -DPACKAGE_VERSION="${GRIVE_VERSION}" )

add_subdirectory( libgrive )
add_subdirectory( grive )
add_subdirectory( bgrive )
