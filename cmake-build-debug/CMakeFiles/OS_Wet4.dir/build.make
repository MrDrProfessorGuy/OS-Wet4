# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/OS_Wet4.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/OS_Wet4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OS_Wet4.dir/flags.make

CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o: CMakeFiles/OS_Wet4.dir/flags.make
CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o: ../malloc_1.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o -c "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_1.cpp"

CMakeFiles/OS_Wet4.dir/malloc_1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OS_Wet4.dir/malloc_1.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_1.cpp" > CMakeFiles/OS_Wet4.dir/malloc_1.cpp.i

CMakeFiles/OS_Wet4.dir/malloc_1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OS_Wet4.dir/malloc_1.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_1.cpp" -o CMakeFiles/OS_Wet4.dir/malloc_1.cpp.s

CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o: CMakeFiles/OS_Wet4.dir/flags.make
CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o: ../malloc_2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o -c "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_2.cpp"

CMakeFiles/OS_Wet4.dir/malloc_2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OS_Wet4.dir/malloc_2.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_2.cpp" > CMakeFiles/OS_Wet4.dir/malloc_2.cpp.i

CMakeFiles/OS_Wet4.dir/malloc_2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OS_Wet4.dir/malloc_2.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_2.cpp" -o CMakeFiles/OS_Wet4.dir/malloc_2.cpp.s

CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o: CMakeFiles/OS_Wet4.dir/flags.make
CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o: ../malloc_3.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o -c "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_3.cpp"

CMakeFiles/OS_Wet4.dir/malloc_3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OS_Wet4.dir/malloc_3.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_3.cpp" > CMakeFiles/OS_Wet4.dir/malloc_3.cpp.i

CMakeFiles/OS_Wet4.dir/malloc_3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OS_Wet4.dir/malloc_3.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/malloc_3.cpp" -o CMakeFiles/OS_Wet4.dir/malloc_3.cpp.s

# Object files for target OS_Wet4
OS_Wet4_OBJECTS = \
"CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o" \
"CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o" \
"CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o"

# External object files for target OS_Wet4
OS_Wet4_EXTERNAL_OBJECTS =

OS_Wet4: CMakeFiles/OS_Wet4.dir/malloc_1.cpp.o
OS_Wet4: CMakeFiles/OS_Wet4.dir/malloc_2.cpp.o
OS_Wet4: CMakeFiles/OS_Wet4.dir/malloc_3.cpp.o
OS_Wet4: CMakeFiles/OS_Wet4.dir/build.make
OS_Wet4: CMakeFiles/OS_Wet4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable OS_Wet4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OS_Wet4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OS_Wet4.dir/build: OS_Wet4

.PHONY : CMakeFiles/OS_Wet4.dir/build

CMakeFiles/OS_Wet4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OS_Wet4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OS_Wet4.dir/clean

CMakeFiles/OS_Wet4.dir/depend:
	cd "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4" "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4" "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug" "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug" "/Users/guycohen/Library/CloudStorage/OneDrive-Technion/1 Courses/Operating Systems/Homework/HW4/Wet/OS-Wet4/cmake-build-debug/CMakeFiles/OS_Wet4.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/OS_Wet4.dir/depend

