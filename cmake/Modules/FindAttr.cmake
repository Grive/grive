find_library( ATTR_LIBRARY 	NAMES attr	PATH /usr/lib /usr/lib64 )

if ( ATTR_LIBRARY )
	set( ATTR_FOUND TRUE )
endif (ATTR_LIBRARY)

if ( ATTR_FOUND )

	message( STATUS "Found libattr: ${ATTR_LIBRARY}")

endif ( ATTR_FOUND )
