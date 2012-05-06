cmake_minimum_required(VERSION 2.8)

add_subdirectory( libgrive )
add_subdirectory( grive )


## Check if QT4 exists before trying to build
find_package(Qt4)
if (QT4_FOUND)
    message("-- Qt4 found: building QGrive")
    add_subdirectory( qgrive )
else (QT4_FOUND)
    message("-- Qt4 not found, the GUI won't be build")
endif(QT4_FOUND)
