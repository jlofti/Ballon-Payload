# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.25.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.25.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/josephloftin/pico/Balloon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/josephloftin/pico/Balloon/build

# Include any dependencies generated for this target.
include CMakeFiles/CommandHandler.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/CommandHandler.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CommandHandler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CommandHandler.dir/flags.make

CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj: CMakeFiles/CommandHandler.dir/flags.make
CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj: /Users/josephloftin/pico/Balloon/src/CommandHandler.cpp
CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj: CMakeFiles/CommandHandler.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/josephloftin/pico/Balloon/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj"
	/usr/local/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj -MF CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj.d -o CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj -c /Users/josephloftin/pico/Balloon/src/CommandHandler.cpp

CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.i"
	/usr/local/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/josephloftin/pico/Balloon/src/CommandHandler.cpp > CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.i

CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.s"
	/usr/local/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/josephloftin/pico/Balloon/src/CommandHandler.cpp -o CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.s

# Object files for target CommandHandler
CommandHandler_OBJECTS = \
"CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj"

# External object files for target CommandHandler
CommandHandler_EXTERNAL_OBJECTS =

libCommandHandler.a: CMakeFiles/CommandHandler.dir/src/CommandHandler.cpp.obj
libCommandHandler.a: CMakeFiles/CommandHandler.dir/build.make
libCommandHandler.a: CMakeFiles/CommandHandler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/josephloftin/pico/Balloon/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libCommandHandler.a"
	$(CMAKE_COMMAND) -P CMakeFiles/CommandHandler.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CommandHandler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CommandHandler.dir/build: libCommandHandler.a
.PHONY : CMakeFiles/CommandHandler.dir/build

CMakeFiles/CommandHandler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CommandHandler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CommandHandler.dir/clean

CMakeFiles/CommandHandler.dir/depend:
	cd /Users/josephloftin/pico/Balloon/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/josephloftin/pico/Balloon /Users/josephloftin/pico/Balloon /Users/josephloftin/pico/Balloon/build /Users/josephloftin/pico/Balloon/build /Users/josephloftin/pico/Balloon/build/CMakeFiles/CommandHandler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CommandHandler.dir/depend
