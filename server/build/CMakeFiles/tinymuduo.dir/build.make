# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wenqingqian/gitrepo/abcd/server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wenqingqian/gitrepo/abcd/server/build

# Include any dependencies generated for this target.
include CMakeFiles/tinymuduo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tinymuduo.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tinymuduo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tinymuduo.dir/flags.make

CMakeFiles/tinymuduo.dir/main.cpp.o: CMakeFiles/tinymuduo.dir/flags.make
CMakeFiles/tinymuduo.dir/main.cpp.o: /home/wenqingqian/gitrepo/abcd/server/main.cpp
CMakeFiles/tinymuduo.dir/main.cpp.o: CMakeFiles/tinymuduo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wenqingqian/gitrepo/abcd/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tinymuduo.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tinymuduo.dir/main.cpp.o -MF CMakeFiles/tinymuduo.dir/main.cpp.o.d -o CMakeFiles/tinymuduo.dir/main.cpp.o -c /home/wenqingqian/gitrepo/abcd/server/main.cpp

CMakeFiles/tinymuduo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tinymuduo.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wenqingqian/gitrepo/abcd/server/main.cpp > CMakeFiles/tinymuduo.dir/main.cpp.i

CMakeFiles/tinymuduo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tinymuduo.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wenqingqian/gitrepo/abcd/server/main.cpp -o CMakeFiles/tinymuduo.dir/main.cpp.s

# Object files for target tinymuduo
tinymuduo_OBJECTS = \
"CMakeFiles/tinymuduo.dir/main.cpp.o"

# External object files for target tinymuduo
tinymuduo_EXTERNAL_OBJECTS =

/home/wenqingqian/gitrepo/abcd/server/tinymuduo: CMakeFiles/tinymuduo.dir/main.cpp.o
/home/wenqingqian/gitrepo/abcd/server/tinymuduo: CMakeFiles/tinymuduo.dir/build.make
/home/wenqingqian/gitrepo/abcd/server/tinymuduo: log/libASYNCLOG.a
/home/wenqingqian/gitrepo/abcd/server/tinymuduo: CMakeFiles/tinymuduo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/wenqingqian/gitrepo/abcd/server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/wenqingqian/gitrepo/abcd/server/tinymuduo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinymuduo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tinymuduo.dir/build: /home/wenqingqian/gitrepo/abcd/server/tinymuduo
.PHONY : CMakeFiles/tinymuduo.dir/build

CMakeFiles/tinymuduo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tinymuduo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tinymuduo.dir/clean

CMakeFiles/tinymuduo.dir/depend:
	cd /home/wenqingqian/gitrepo/abcd/server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wenqingqian/gitrepo/abcd/server /home/wenqingqian/gitrepo/abcd/server /home/wenqingqian/gitrepo/abcd/server/build /home/wenqingqian/gitrepo/abcd/server/build /home/wenqingqian/gitrepo/abcd/server/build/CMakeFiles/tinymuduo.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/tinymuduo.dir/depend
