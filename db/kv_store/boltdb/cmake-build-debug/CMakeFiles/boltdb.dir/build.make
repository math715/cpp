# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /home/ruoshui/software/clion/clion-2017.3.4/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/ruoshui/software/clion/clion-2017.3.4/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ruoshui/study/cpp/db/kv_store/boltdb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/boltdb.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/boltdb.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/boltdb.dir/flags.make

CMakeFiles/boltdb.dir/db.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/db.cc.o: ../db.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/boltdb.dir/db.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/db.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/db.cc

CMakeFiles/boltdb.dir/db.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/db.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/db.cc > CMakeFiles/boltdb.dir/db.cc.i

CMakeFiles/boltdb.dir/db.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/db.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/db.cc -o CMakeFiles/boltdb.dir/db.cc.s

CMakeFiles/boltdb.dir/db.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/db.cc.o.requires

CMakeFiles/boltdb.dir/db.cc.o.provides: CMakeFiles/boltdb.dir/db.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/db.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/db.cc.o.provides

CMakeFiles/boltdb.dir/db.cc.o.provides.build: CMakeFiles/boltdb.dir/db.cc.o


CMakeFiles/boltdb.dir/error.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/error.cc.o: ../error.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/boltdb.dir/error.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/error.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/error.cc

CMakeFiles/boltdb.dir/error.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/error.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/error.cc > CMakeFiles/boltdb.dir/error.cc.i

CMakeFiles/boltdb.dir/error.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/error.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/error.cc -o CMakeFiles/boltdb.dir/error.cc.s

CMakeFiles/boltdb.dir/error.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/error.cc.o.requires

CMakeFiles/boltdb.dir/error.cc.o.provides: CMakeFiles/boltdb.dir/error.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/error.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/error.cc.o.provides

CMakeFiles/boltdb.dir/error.cc.o.provides.build: CMakeFiles/boltdb.dir/error.cc.o


CMakeFiles/boltdb.dir/bucket.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/bucket.cc.o: ../bucket.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/boltdb.dir/bucket.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/bucket.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/bucket.cc

CMakeFiles/boltdb.dir/bucket.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/bucket.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/bucket.cc > CMakeFiles/boltdb.dir/bucket.cc.i

CMakeFiles/boltdb.dir/bucket.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/bucket.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/bucket.cc -o CMakeFiles/boltdb.dir/bucket.cc.s

CMakeFiles/boltdb.dir/bucket.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/bucket.cc.o.requires

CMakeFiles/boltdb.dir/bucket.cc.o.provides: CMakeFiles/boltdb.dir/bucket.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/bucket.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/bucket.cc.o.provides

CMakeFiles/boltdb.dir/bucket.cc.o.provides.build: CMakeFiles/boltdb.dir/bucket.cc.o


CMakeFiles/boltdb.dir/Tx.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/Tx.cc.o: ../Tx.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/boltdb.dir/Tx.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/Tx.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/Tx.cc

CMakeFiles/boltdb.dir/Tx.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/Tx.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/Tx.cc > CMakeFiles/boltdb.dir/Tx.cc.i

CMakeFiles/boltdb.dir/Tx.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/Tx.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/Tx.cc -o CMakeFiles/boltdb.dir/Tx.cc.s

CMakeFiles/boltdb.dir/Tx.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/Tx.cc.o.requires

CMakeFiles/boltdb.dir/Tx.cc.o.provides: CMakeFiles/boltdb.dir/Tx.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/Tx.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/Tx.cc.o.provides

CMakeFiles/boltdb.dir/Tx.cc.o.provides.build: CMakeFiles/boltdb.dir/Tx.cc.o


CMakeFiles/boltdb.dir/page.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/page.cc.o: ../page.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/boltdb.dir/page.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/page.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/page.cc

CMakeFiles/boltdb.dir/page.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/page.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/page.cc > CMakeFiles/boltdb.dir/page.cc.i

CMakeFiles/boltdb.dir/page.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/page.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/page.cc -o CMakeFiles/boltdb.dir/page.cc.s

CMakeFiles/boltdb.dir/page.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/page.cc.o.requires

CMakeFiles/boltdb.dir/page.cc.o.provides: CMakeFiles/boltdb.dir/page.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/page.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/page.cc.o.provides

CMakeFiles/boltdb.dir/page.cc.o.provides.build: CMakeFiles/boltdb.dir/page.cc.o


CMakeFiles/boltdb.dir/freelist.cc.o: CMakeFiles/boltdb.dir/flags.make
CMakeFiles/boltdb.dir/freelist.cc.o: ../freelist.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/boltdb.dir/freelist.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/boltdb.dir/freelist.cc.o -c /home/ruoshui/study/cpp/db/kv_store/boltdb/freelist.cc

CMakeFiles/boltdb.dir/freelist.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/boltdb.dir/freelist.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ruoshui/study/cpp/db/kv_store/boltdb/freelist.cc > CMakeFiles/boltdb.dir/freelist.cc.i

CMakeFiles/boltdb.dir/freelist.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/boltdb.dir/freelist.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ruoshui/study/cpp/db/kv_store/boltdb/freelist.cc -o CMakeFiles/boltdb.dir/freelist.cc.s

CMakeFiles/boltdb.dir/freelist.cc.o.requires:

.PHONY : CMakeFiles/boltdb.dir/freelist.cc.o.requires

CMakeFiles/boltdb.dir/freelist.cc.o.provides: CMakeFiles/boltdb.dir/freelist.cc.o.requires
	$(MAKE) -f CMakeFiles/boltdb.dir/build.make CMakeFiles/boltdb.dir/freelist.cc.o.provides.build
.PHONY : CMakeFiles/boltdb.dir/freelist.cc.o.provides

CMakeFiles/boltdb.dir/freelist.cc.o.provides.build: CMakeFiles/boltdb.dir/freelist.cc.o


# Object files for target boltdb
boltdb_OBJECTS = \
"CMakeFiles/boltdb.dir/db.cc.o" \
"CMakeFiles/boltdb.dir/error.cc.o" \
"CMakeFiles/boltdb.dir/bucket.cc.o" \
"CMakeFiles/boltdb.dir/Tx.cc.o" \
"CMakeFiles/boltdb.dir/page.cc.o" \
"CMakeFiles/boltdb.dir/freelist.cc.o"

# External object files for target boltdb
boltdb_EXTERNAL_OBJECTS =

boltdb: CMakeFiles/boltdb.dir/db.cc.o
boltdb: CMakeFiles/boltdb.dir/error.cc.o
boltdb: CMakeFiles/boltdb.dir/bucket.cc.o
boltdb: CMakeFiles/boltdb.dir/Tx.cc.o
boltdb: CMakeFiles/boltdb.dir/page.cc.o
boltdb: CMakeFiles/boltdb.dir/freelist.cc.o
boltdb: CMakeFiles/boltdb.dir/build.make
boltdb: CMakeFiles/boltdb.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable boltdb"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/boltdb.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/boltdb.dir/build: boltdb

.PHONY : CMakeFiles/boltdb.dir/build

CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/db.cc.o.requires
CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/error.cc.o.requires
CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/bucket.cc.o.requires
CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/Tx.cc.o.requires
CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/page.cc.o.requires
CMakeFiles/boltdb.dir/requires: CMakeFiles/boltdb.dir/freelist.cc.o.requires

.PHONY : CMakeFiles/boltdb.dir/requires

CMakeFiles/boltdb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/boltdb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/boltdb.dir/clean

CMakeFiles/boltdb.dir/depend:
	cd /home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ruoshui/study/cpp/db/kv_store/boltdb /home/ruoshui/study/cpp/db/kv_store/boltdb /home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug /home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug /home/ruoshui/study/cpp/db/kv_store/boltdb/cmake-build-debug/CMakeFiles/boltdb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/boltdb.dir/depend

