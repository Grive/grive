# - Try to find Libnotify
# Once done this will define
#
#  LIBNOTIFY_FOUND - system has Libnotify
#  LIBNOTIFY_INCLUDE_DIR - the Libnotify include directory
#  LIBNOTIFY_LIBRARIES - Link these to use Libnotify
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Copyright (c) 2010, Ni Hui <shuizhuyuanluo@126.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(LIBNOTIFY_INCLUDE_DIR AND LIBNOTIFY_LIBRARIES)
    # in cache already
    set(LIBNOTIFY_FOUND_QUIETLY TRUE)
endif(LIBNOTIFY_INCLUDE_DIR AND LIBNOTIFY_LIBRARIES)

if(NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PKG_LIBNOTIFY libnotify)
    if(PKG_LIBNOTIFY_FOUND)
        set(LIBNOTIFY_FOUND)
    endif(PKG_LIBNOTIFY_FOUND)
endif(NOT WIN32)

find_path(LIBNOTIFY_INCLUDE_DIR NAMES notify.h
    PATHS
    HINTS ${PKG_LIBNOTIFY_INCLUDE_DIRS}
    PATH_SUFFIXES libnotify
)

find_library(LIBNOTIFY_LIBRARIES NAMES notify
    PATHS
    HINTS ${PKG_LIBNOTIFY_LIBRARY_DIRS}
)

if(LIBNOTIFY_INCLUDE_DIR AND LIBNOTIFY_LIBRARIES)
    set(LIBNOTIFY_FOUND TRUE)
else(LIBNOTIFY_INCLUDE_DIR AND LIBNOTIFY_LIBRARIES)
    set(LIBNOTIFY_FOUND FALSE)
endif(LIBNOTIFY_INCLUDE_DIR AND LIBNOTIFY_LIBRARIES)

if(LIBNOTIFY_FOUND)
    include(CheckCSourceCompiles)    
    find_package(GTK2 REQUIRED)
    # check whether we are using libnotify 0.4
    set(CMAKE_REQUIRED_INCLUDES ${LIBNOTIFY_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR} ${GLIB2_INTERNALE_DIR} ${GTK2_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${LIBNOTIFY_LIBRARIES} ${GLIB2_LIBRARIES} ${GTK2_LIBRARIES})
check_c_source_compiles("
#include <notify.h>

int main()
{
    notify_notification_new(NULL,NULL,NULL,NULL);
    return 0;
}
" HAVE_LIBNOTIFY_0_4)
check_c_source_compiles("
#include <notify.h>

int main()
{
    notify_notification_new(NULL,NULL,NULL);
    return 0;
}
" HAVE_LIBNOTIFY_0_7)
    set(CMAKE_REQUIRED_INCLUDES)
    set(CMAKE_REQUIRED_LIBRARIES)
    if(HAVE_LIBNOTIFY_0_4)
        message(STATUS "Found Libnotify: ${LIBNOTIFY_LIBRARIES}, (>= 0.4)")
    endif(HAVE_LIBNOTIFY_0_4)
    if(HAVE_LIBNOTIFY_0_7)
        message(STATUS "Found Libnotify: ${LIBNOTIFY_LIBRARIES}, (>= 0.7)")
    endif(HAVE_LIBNOTIFY_0_7)
endif(LIBNOTIFY_FOUND)

set(LIBNOTIFY_INCLUDE_DIR ${LIBNOTIFY_INCLUDE_DIR} CACHE INTERNAL "The Libnotify include path")
set(LIBNOTIFY_LIBRARIES ${LIBNOTIFY_LIBRARIES} CACHE INTERNAL "The Libnotify library")
set(HAVE_LIBNOTIFY_0_4 ${HAVE_LIBNOTIFY_0_4} CACHE INTERNAL "Whether the version of Libnotify is >= 0.4")
set(HAVE_LIBNOTIFY_0_7 ${HAVE_LIBNOTIFY_0_7} CACHE INTERNAL "Whether the version of Libnotify is >= 0.7")

mark_as_advanced(LIBNOTIFY_INCLUDE_DIR LIBNOTIFY_LIBRARIES)