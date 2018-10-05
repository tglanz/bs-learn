# the minimum version of cmake according to bsf
cmake_minimum_required (VERSION 3.9.0)

# the name of the project
project (bs-learn)

# let the system know where bsf source code is
set (BSF_DIRECTORY "d:/src/bsf" CACHE STRING "bsf directory")

# include bsf projects and their CMake
include (${BSF_DIRECTORY}/Source/CMake/ProjectDefaults.cmake)

# build an executable
add_executable (app "main.cpp")

# add bsf as dependency
target_link_libraries (app bsf)

# add required plugin bsf dependencies
add_engine_dependencies (app)

# add optional dependencies
add_dependencies (app bsfFBXImporter bsfFontImporter bsfFreeImgImporter)

# when app is build, copy required binaries from bsf
copyBsfBinaries (app ${BSF_DIRECTORY})
