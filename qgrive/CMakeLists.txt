## QGrive, Qt interface for libgrive
project ( qgrive )

## Declare sources and headers
file( GLOB QGRIVE_SOURCES
    src/*.cc
)

file( GLOB QGRIVE_HEADERS
    include/*.hh
)

include( ${QT_USE_FILE} )
include_directories(
    ${libgrive_SOURCE_DIR}/src
    ${qgrive_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions( ${QT_DEFINITIONS} )

## Generate moc files
qt4_wrap_cpp ( QGRIVE_HEADERS_MOC ${QGRIVE_HEADERS} )

## Generate the .ui file
qt4_wrap_ui ( QGRIVE_HEADERS_UI ui/mainwindow.ui )

add_executable( qgrive ${QGRIVE_SOURCES} ${QGRIVE_HEADERS_MOC} ${QGRIVE_HEADERS_UI} )
target_link_libraries( qgrive ${QT_LIBRARIES} grive )