# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jpuchky/Projects/grive

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jpuchky/Projects/grive

# Include any dependencies generated for this target.
include libgrive/CMakeFiles/btest.dir/depend.make

# Include the progress variables for this target.
include libgrive/CMakeFiles/btest.dir/progress.make

# Include the compile flags for this target's objects.
include libgrive/CMakeFiles/btest.dir/flags.make

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o: libgrive/CMakeFiles/btest.dir/flags.make
libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o: libgrive/test/btest/JsonValTest.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jpuchky/Projects/grive/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -o CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o -c /home/jpuchky/Projects/grive/libgrive/test/btest/JsonValTest.cc

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/btest.dir/test/btest/JsonValTest.cc.i"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -E /home/jpuchky/Projects/grive/libgrive/test/btest/JsonValTest.cc > CMakeFiles/btest.dir/test/btest/JsonValTest.cc.i

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/btest.dir/test/btest/JsonValTest.cc.s"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -S /home/jpuchky/Projects/grive/libgrive/test/btest/JsonValTest.cc -o CMakeFiles/btest.dir/test/btest/JsonValTest.cc.s

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.requires:
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.requires

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.provides: libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.requires
	$(MAKE) -f libgrive/CMakeFiles/btest.dir/build.make libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.provides.build
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.provides

libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.provides.build: libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o: libgrive/CMakeFiles/btest.dir/flags.make
libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o: libgrive/test/btest/UnitTest.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jpuchky/Projects/grive/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -o CMakeFiles/btest.dir/test/btest/UnitTest.cc.o -c /home/jpuchky/Projects/grive/libgrive/test/btest/UnitTest.cc

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/btest.dir/test/btest/UnitTest.cc.i"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -E /home/jpuchky/Projects/grive/libgrive/test/btest/UnitTest.cc > CMakeFiles/btest.dir/test/btest/UnitTest.cc.i

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/btest.dir/test/btest/UnitTest.cc.s"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -S /home/jpuchky/Projects/grive/libgrive/test/btest/UnitTest.cc -o CMakeFiles/btest.dir/test/btest/UnitTest.cc.s

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.requires:
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.requires

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.provides: libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.requires
	$(MAKE) -f libgrive/CMakeFiles/btest.dir/build.make libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.provides.build
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.provides

libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.provides.build: libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o: libgrive/CMakeFiles/btest.dir/flags.make
libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o: libgrive/test/btest/ValTest.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jpuchky/Projects/grive/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -o CMakeFiles/btest.dir/test/btest/ValTest.cc.o -c /home/jpuchky/Projects/grive/libgrive/test/btest/ValTest.cc

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/btest.dir/test/btest/ValTest.cc.i"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -E /home/jpuchky/Projects/grive/libgrive/test/btest/ValTest.cc > CMakeFiles/btest.dir/test/btest/ValTest.cc.i

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/btest.dir/test/btest/ValTest.cc.s"
	cd /home/jpuchky/Projects/grive/libgrive && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -DBOOST_TEST_DYN_LINK -S /home/jpuchky/Projects/grive/libgrive/test/btest/ValTest.cc -o CMakeFiles/btest.dir/test/btest/ValTest.cc.s

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.requires:
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.requires

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.provides: libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.requires
	$(MAKE) -f libgrive/CMakeFiles/btest.dir/build.make libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.provides.build
.PHONY : libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.provides

libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.provides.build: libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o

# Object files for target btest
btest_OBJECTS = \
"CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o" \
"CMakeFiles/btest.dir/test/btest/UnitTest.cc.o" \
"CMakeFiles/btest.dir/test/btest/ValTest.cc.o"

# External object files for target btest
btest_EXTERNAL_OBJECTS =

libgrive/btest: libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o
libgrive/btest: libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o
libgrive/btest: libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o
libgrive/btest: libgrive/CMakeFiles/btest.dir/build.make
libgrive/btest: libgrive/libgrive.a
libgrive/btest: /usr/lib64/libboost_program_options-mt.so
libgrive/btest: /usr/lib64/libboost_filesystem-mt.so
libgrive/btest: /usr/lib64/libboost_unit_test_framework-mt.so
libgrive/btest: /usr/lib64/libboost_system-mt.so
libgrive/btest: /usr/lib64/libcurl.so
libgrive/btest: /usr/lib64/libjson.so
libgrive/btest: /usr/lib64/libboost_program_options-mt.so
libgrive/btest: /usr/lib64/libboost_filesystem-mt.so
libgrive/btest: /usr/lib64/libboost_unit_test_framework-mt.so
libgrive/btest: /usr/lib64/libboost_system-mt.so
libgrive/btest: /usr/lib64/libiberty.a
libgrive/btest: /usr/lib64/libexpat.so
libgrive/btest: /usr/lib64/libdl.so
libgrive/btest: /usr/lib64/libbfd.so
libgrive/btest: /usr/lib64/libiberty.a
libgrive/btest: /usr/lib64/libexpat.so
libgrive/btest: /usr/lib64/libdl.so
libgrive/btest: /usr/lib64/libbfd.so
libgrive/btest: /usr/lib64/libz.so
libgrive/btest: libgrive/CMakeFiles/btest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable btest"
	cd /home/jpuchky/Projects/grive/libgrive && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/btest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libgrive/CMakeFiles/btest.dir/build: libgrive/btest
.PHONY : libgrive/CMakeFiles/btest.dir/build

libgrive/CMakeFiles/btest.dir/requires: libgrive/CMakeFiles/btest.dir/test/btest/JsonValTest.cc.o.requires
libgrive/CMakeFiles/btest.dir/requires: libgrive/CMakeFiles/btest.dir/test/btest/UnitTest.cc.o.requires
libgrive/CMakeFiles/btest.dir/requires: libgrive/CMakeFiles/btest.dir/test/btest/ValTest.cc.o.requires
.PHONY : libgrive/CMakeFiles/btest.dir/requires

libgrive/CMakeFiles/btest.dir/clean:
	cd /home/jpuchky/Projects/grive/libgrive && $(CMAKE_COMMAND) -P CMakeFiles/btest.dir/cmake_clean.cmake
.PHONY : libgrive/CMakeFiles/btest.dir/clean

libgrive/CMakeFiles/btest.dir/depend:
	cd /home/jpuchky/Projects/grive && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jpuchky/Projects/grive /home/jpuchky/Projects/grive/libgrive /home/jpuchky/Projects/grive /home/jpuchky/Projects/grive/libgrive /home/jpuchky/Projects/grive/libgrive/CMakeFiles/btest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libgrive/CMakeFiles/btest.dir/depend

