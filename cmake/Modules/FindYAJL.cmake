# - Find YAJL
# This module finds an installed YAJL package.
#
# It sets the following variables:
#  YAJL_FOUND       - Set to false, or undefined, if YAJL isn't found.
#  YAJL_INCLUDE_DIR - The YAJL include directory.
#  YAJL_LIBRARY     - The YAJL library to link against.

FIND_PATH(YAJL_INCLUDE_DIR yajl/yajl_gen.h)
FIND_LIBRARY(YAJL_LIBRARY NAMES yajl)

IF (YAJL_INCLUDE_DIR AND YAJL_LIBRARY)
   SET(YAJL_FOUND TRUE)
ENDIF (YAJL_INCLUDE_DIR AND YAJL_LIBRARY)

IF (YAJL_FOUND)

   # show which YAJL was found only if not quiet
   IF (NOT YAJL_FIND_QUIETLY)
      MESSAGE(STATUS "Found YAJL: ${YAJL_LIBRARY}")
   ENDIF (NOT YAJL_FIND_QUIETLY)

ELSE (YAJL_FOUND)

   # fatal error if YAJL is required but not found
   IF (YAJL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find YAJL")
   ENDIF (YAJL_FIND_REQUIRED)

ENDIF (YAJL_FOUND)

