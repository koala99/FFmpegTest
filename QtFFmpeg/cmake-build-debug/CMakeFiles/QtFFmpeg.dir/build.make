# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/lilei/cpp_works/QtFFmpeg

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/QtFFmpeg.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/QtFFmpeg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/QtFFmpeg.dir/flags.make

CMakeFiles/QtFFmpeg.dir/main.cpp.o: CMakeFiles/QtFFmpeg.dir/flags.make
CMakeFiles/QtFFmpeg.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/QtFFmpeg.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtFFmpeg.dir/main.cpp.o -c /Users/lilei/cpp_works/QtFFmpeg/main.cpp

CMakeFiles/QtFFmpeg.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtFFmpeg.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lilei/cpp_works/QtFFmpeg/main.cpp > CMakeFiles/QtFFmpeg.dir/main.cpp.i

CMakeFiles/QtFFmpeg.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtFFmpeg.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lilei/cpp_works/QtFFmpeg/main.cpp -o CMakeFiles/QtFFmpeg.dir/main.cpp.s

CMakeFiles/QtFFmpeg.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/QtFFmpeg.dir/main.cpp.o.requires

CMakeFiles/QtFFmpeg.dir/main.cpp.o.provides: CMakeFiles/QtFFmpeg.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/QtFFmpeg.dir/build.make CMakeFiles/QtFFmpeg.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/QtFFmpeg.dir/main.cpp.o.provides

CMakeFiles/QtFFmpeg.dir/main.cpp.o.provides.build: CMakeFiles/QtFFmpeg.dir/main.cpp.o


CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o: CMakeFiles/QtFFmpeg.dir/flags.make
CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o: ../VideoWidget.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o -c /Users/lilei/cpp_works/QtFFmpeg/VideoWidget.cpp

CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lilei/cpp_works/QtFFmpeg/VideoWidget.cpp > CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.i

CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lilei/cpp_works/QtFFmpeg/VideoWidget.cpp -o CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.s

CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.requires:

.PHONY : CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.requires

CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.provides: CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.requires
	$(MAKE) -f CMakeFiles/QtFFmpeg.dir/build.make CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.provides.build
.PHONY : CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.provides

CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.provides.build: CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o


CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o: CMakeFiles/QtFFmpeg.dir/flags.make
CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o: ../FFmpegUtils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o -c /Users/lilei/cpp_works/QtFFmpeg/FFmpegUtils.cpp

CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lilei/cpp_works/QtFFmpeg/FFmpegUtils.cpp > CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.i

CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lilei/cpp_works/QtFFmpeg/FFmpegUtils.cpp -o CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.s

CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.requires:

.PHONY : CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.requires

CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.provides: CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.requires
	$(MAKE) -f CMakeFiles/QtFFmpeg.dir/build.make CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.provides.build
.PHONY : CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.provides

CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.provides.build: CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o


CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o: CMakeFiles/QtFFmpeg.dir/flags.make
CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o: QtFFmpeg_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o -c /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/QtFFmpeg_autogen/mocs_compilation.cpp

CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/QtFFmpeg_autogen/mocs_compilation.cpp > CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.i

CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/QtFFmpeg_autogen/mocs_compilation.cpp -o CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.s

CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.requires:

.PHONY : CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.requires

CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.provides: CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.requires
	$(MAKE) -f CMakeFiles/QtFFmpeg.dir/build.make CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.provides.build
.PHONY : CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.provides

CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.provides.build: CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o


# Object files for target QtFFmpeg
QtFFmpeg_OBJECTS = \
"CMakeFiles/QtFFmpeg.dir/main.cpp.o" \
"CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o" \
"CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o" \
"CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o"

# External object files for target QtFFmpeg
QtFFmpeg_EXTERNAL_OBJECTS =

QtFFmpeg: CMakeFiles/QtFFmpeg.dir/main.cpp.o
QtFFmpeg: CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o
QtFFmpeg: CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o
QtFFmpeg: CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o
QtFFmpeg: CMakeFiles/QtFFmpeg.dir/build.make
QtFFmpeg: /usr/local/Cellar/qt/5.10.0/lib/QtWidgets.framework/QtWidgets
QtFFmpeg: /usr/local/Cellar/qt/5.10.0/lib/QtGui.framework/QtGui
QtFFmpeg: /usr/local/Cellar/qt/5.10.0/lib/QtCore.framework/QtCore
QtFFmpeg: CMakeFiles/QtFFmpeg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable QtFFmpeg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/QtFFmpeg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/QtFFmpeg.dir/build: QtFFmpeg

.PHONY : CMakeFiles/QtFFmpeg.dir/build

CMakeFiles/QtFFmpeg.dir/requires: CMakeFiles/QtFFmpeg.dir/main.cpp.o.requires
CMakeFiles/QtFFmpeg.dir/requires: CMakeFiles/QtFFmpeg.dir/VideoWidget.cpp.o.requires
CMakeFiles/QtFFmpeg.dir/requires: CMakeFiles/QtFFmpeg.dir/FFmpegUtils.cpp.o.requires
CMakeFiles/QtFFmpeg.dir/requires: CMakeFiles/QtFFmpeg.dir/QtFFmpeg_autogen/mocs_compilation.cpp.o.requires

.PHONY : CMakeFiles/QtFFmpeg.dir/requires

CMakeFiles/QtFFmpeg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/QtFFmpeg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/QtFFmpeg.dir/clean

CMakeFiles/QtFFmpeg.dir/depend:
	cd /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/lilei/cpp_works/QtFFmpeg /Users/lilei/cpp_works/QtFFmpeg /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug /Users/lilei/cpp_works/QtFFmpeg/cmake-build-debug/CMakeFiles/QtFFmpeg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/QtFFmpeg.dir/depend

