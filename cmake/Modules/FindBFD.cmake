find_library( DL_LIBRARY 	NAMES dl	PATH /usr/lib /usr/lib64 )
find_library( BFD_LIBRARY	NAMES bfd	PATH /usr/lib /usr/lib64 )

if ( DL_LIBRARY AND BFD_LIBRARY )
	set( BFD_FOUND TRUE )
endif (DL_LIBRARY AND BFD_LIBRARY)

if ( BFD_FOUND )

	message( STATUS "Found libbfd: ${BFD_LIBRARY}")


endif ( BFD_FOUND )
