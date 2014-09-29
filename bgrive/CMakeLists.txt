project( bgrive )

find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)
INCLUDE(${QT_USE_FILE})

include_directories(
	${bgrive_SOURCE_DIR}/../libgrive/src
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

file (GLOB BGRIVE_EXE_SRC
    ${bgrive_SOURCE_DIR}/src/*.cc
)

file (GLOB BGRIVE_UI
	${bgrive_SOURCE_DIR}/ui/*.ui
)

QT4_WRAP_UI(BGRIVE_UI_SRCS
	${BGRIVE_UI}
)
QT4_WRAP_CPP(BGRIVE_MOC_SRCS
	src/MainWnd.hh
)

add_executable( bgrive_executable
	${BGRIVE_EXE_SRC}
	${BGRIVE_UI_SRCS}
	${BGRIVE_MOC_SRCS}
)

target_link_libraries( bgrive_executable
	${Boost_LIBRARIES}
	${QT_QTMAIN_LIBRARY}
	${QT_LIBRARIES}
    grive
)

set_target_properties( bgrive_executable
    PROPERTIES OUTPUT_NAME bgrive
)

install(TARGETS bgrive_executable RUNTIME DESTINATION bin)
