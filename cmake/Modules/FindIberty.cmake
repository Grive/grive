# - Find Iberty
# This module finds libiberty.
#
# It sets the following variables:
#  IBERTY_LIBRARY     - The library to link against.

FIND_LIBRARY( IBERTY_LIBRARY NAMES iberty )

IF (IBERTY_LIBRARY)

	# show which library was found only if not quiet
	MESSAGE( STATUS "Found libiberty: ${IBERTY_LIBRARY}")
	
	SET(IBERTY_FOUND TRUE)

ELSE (IBERTY_LIBRARY)

   IF ( IBERTY_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libiberty. try to install binutil-devel?")
   ENDIF (IBERTY_FIND_REQUIRED)

ENDIF (IBERTY_LIBRARY)
