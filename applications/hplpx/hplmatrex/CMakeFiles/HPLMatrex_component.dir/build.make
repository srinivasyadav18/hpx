# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bufo/projects/hpx_head

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bufo/projects/hpx_head

# Include any dependencies generated for this target.
include applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/depend.make

# Include the progress variables for this target.
include applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/progress.make

# Include the compile flags for this target's objects.
include applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/flags.make

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/flags.make
applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o: applications/hplpx/hplmatrex/hplmatrex.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bufo/projects/hpx_head/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o"
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o -c /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex/hplmatrex.cpp

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.i"
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex/hplmatrex.cpp > CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.i

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.s"
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex/hplmatrex.cpp -o CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.s

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.requires:
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.requires

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.provides: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.requires
	$(MAKE) -f applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/build.make applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.provides.build
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.provides

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.provides.build: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.provides.build

# Object files for target HPLMatrex_component
HPLMatrex_component_OBJECTS = \
"CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o"

# External object files for target HPLMatrex_component
HPLMatrex_component_EXTERNAL_OBJECTS =

applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: src/libhpx.so.0.2.0
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: src/libhpx_serialization.so.0.2.0
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_date_time.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_filesystem.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_program_options.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_regex.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_serialization.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_system.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_signals.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: /home/bufo/usr/local/boost/lib/libboost_thread.so
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/build.make
applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library libhpx_component_HPLMatrex.so"
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/HPLMatrex_component.dir/link.txt --verbose=$(VERBOSE)
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && $(CMAKE_COMMAND) -E cmake_symlink_library libhpx_component_HPLMatrex.so.0.2.0 libhpx_component_HPLMatrex.so.0 libhpx_component_HPLMatrex.so

applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0: applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0

applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so: applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so.0.2.0

# Rule to build all files generated by this target.
applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/build: applications/hplpx/hplmatrex/libhpx_component_HPLMatrex.so
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/build

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/requires: applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/hplmatrex.cpp.o.requires
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/requires

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/clean:
	cd /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex && $(CMAKE_COMMAND) -P CMakeFiles/HPLMatrex_component.dir/cmake_clean.cmake
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/clean

applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/depend:
	cd /home/bufo/projects/hpx_head && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bufo/projects/hpx_head /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex /home/bufo/projects/hpx_head /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex /home/bufo/projects/hpx_head/applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : applications/hplpx/hplmatrex/CMakeFiles/HPLMatrex_component.dir/depend

