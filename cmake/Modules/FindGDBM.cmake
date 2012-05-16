# - Find gdbm
# Find the native GDBM includes and library
#
#  GDBM_INCLUDE_DIR - where to find gdbm.h, etc.
#  GDBM_LIBRARIES   - List of libraries when using gdbm.
#  GDBM_FOUND       - True if gdbm found.


IF (GDBM_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GDBM_FIND_QUIETLY TRUE)
ENDIF (GDBM_INCLUDE_DIR)

FIND_PATH(GDBM_INCLUDE_DIR gdbm.h
  /usr/local/include
  /usr/include
  /opt/local/include
)

SET(GDBM_NAMES gdbm)
FIND_LIBRARY(GDBM_LIBRARY
  NAMES ${GDBM_NAMES}
  PATHS /usr/lib /usr/local/lib /opt/local/lib
)

IF (GDBM_INCLUDE_DIR AND GDBM_LIBRARY)
   SET(GDBM_FOUND TRUE)
    SET( GDBM_LIBRARIES ${GDBM_LIBRARY} )
ELSE (GDBM_INCLUDE_DIR AND GDBM_LIBRARY)
   SET(GDBM_FOUND FALSE)
   SET( GDBM_LIBRARIES )
ENDIF (GDBM_INCLUDE_DIR AND GDBM_LIBRARY)

IF (GDBM_FOUND)
   IF (NOT GDBM_FIND_QUIETLY)
      MESSAGE(STATUS "Found GDBM: ${GDBM_LIBRARY}")
   ENDIF (NOT GDBM_FIND_QUIETLY)
ELSE (GDBM_FOUND)
   IF (GDBM_FIND_REQUIRED)
      MESSAGE(STATUS "Looked for gdbm libraries named ${GDBMS_NAMES}.")
      MESSAGE(FATAL_ERROR "Could NOT find gdbm library")
   ENDIF (GDBM_FIND_REQUIRED)
ENDIF (GDBM_FOUND)

MARK_AS_ADVANCED(
  GDBM_LIBRARY
  GDBM_INCLUDE_DIR
)
