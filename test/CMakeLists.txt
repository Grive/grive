# test subfolder

file(GLOB UTIL_TESTS util/*.cc)


add_executable( unittest
    UnitTest.cc
    ${UTIL_TESTS}
)

target_link_libraries(unittest
    grive
    ${CPPUNIT_LIBRARY}
)
