# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /opt/clion-2020.1.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2020.1.1/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/mina/JetBrains Project/Clion/onepassassembler"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/onepassassembler.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/onepassassembler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/onepassassembler.dir/flags.make

CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o: CMakeFiles/onepassassembler.dir/flags.make
CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o: ../Assembler/src/assembler.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o -c "/home/mina/JetBrains Project/Clion/onepassassembler/Assembler/src/assembler.cpp"

CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/mina/JetBrains Project/Clion/onepassassembler/Assembler/src/assembler.cpp" > CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.i

CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/mina/JetBrains Project/Clion/onepassassembler/Assembler/src/assembler.cpp" -o CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.s

# Object files for target onepassassembler
onepassassembler_OBJECTS = \
"CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o"

# External object files for target onepassassembler
onepassassembler_EXTERNAL_OBJECTS =

onepassassembler: CMakeFiles/onepassassembler.dir/Assembler/src/assembler.cpp.o
onepassassembler: CMakeFiles/onepassassembler.dir/build.make
onepassassembler: CMakeFiles/onepassassembler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable onepassassembler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/onepassassembler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/onepassassembler.dir/build: onepassassembler

.PHONY : CMakeFiles/onepassassembler.dir/build

CMakeFiles/onepassassembler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/onepassassembler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/onepassassembler.dir/clean

CMakeFiles/onepassassembler.dir/depend:
	cd "/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/mina/JetBrains Project/Clion/onepassassembler" "/home/mina/JetBrains Project/Clion/onepassassembler" "/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug" "/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug" "/home/mina/JetBrains Project/Clion/onepassassembler/cmake-build-debug/CMakeFiles/onepassassembler.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/onepassassembler.dir/depend
