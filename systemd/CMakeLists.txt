SET(GRIVE_SYNC_SH_BINARY "${CMAKE_INSTALL_PREFIX}/lib/grive/grive-sync.sh")

CONFIGURE_FILE(grive-changes@.service.in grive-changes@.service @ONLY)
CONFIGURE_FILE(grive-timer@.service.in grive-timer@.service @ONLY)

install(
  FILES
    ${CMAKE_BINARY_DIR}/systemd/grive-changes@.service
    ${CMAKE_BINARY_DIR}/systemd/grive-timer@.service
  DESTINATION
    lib/systemd/user
)

install(
  FILES
    grive-timer@.timer
  DESTINATION
    lib/systemd/user
)

install(
  PROGRAMS
    grive-sync.sh
  DESTINATION
    lib/grive
)