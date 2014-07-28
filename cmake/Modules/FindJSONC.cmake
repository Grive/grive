# - Find JSON-C
# This module finds an installed JSON-C package.
#
# It sets the following variables:
#  JSONC_FOUND       - Set to false, or undefined, if JSON-C isn't found.
#  JSONC_INCLUDE_DIR - The JSON-C include directory.
#  JSONC_LIBRARY     - The JSON-C library to link against.

FIND_PATH(JSONC_INCLUDE_DIR json.h PATH_SUFFIXES json json-c)
FIND_LIBRARY(JSONC_LIBRARY NAMES json json-c)

IF (JSONC_INCLUDE_DIR AND JSONC_LIBRARY)
   SET(JSONC_FOUND TRUE)
ENDIF (JSONC_INCLUDE_DIR AND JSONC_LIBRARY)

IF (JSONC_FOUND)

   # show which JSON-C was found only if not quiet
   IF (NOT JSONC_FIND_QUIETLY)
      MESSAGE(STATUS "Found JSON-C: ${JSONC_LIBRARY}")
   ENDIF (NOT JSONC_FIND_QUIETLY)

ELSE (JSONC_FOUND)

   # fatal error if JSON-C is required but not found
   IF (JSONC_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find JSON-C")
   ENDIF (JSONC_FIND_REQUIRED)

ENDIF (JSONC_FOUND)
