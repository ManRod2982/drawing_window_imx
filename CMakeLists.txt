# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 3.10)
# Set the name and the supported language of the project
project(window)
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# Create the executable
add_executable(window)
target_sources(window
    PRIVATE
    src/main.cpp
    src/window.cpp
    src/mouse_drawing.cpp
)
target_link_libraries(window PRIVATE ${GTKMM_LIBRARIES})

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
target_include_directories(window
    PRIVATE ${GTKMM_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_directories(window PRIVATE ${GTKMM_LIBRARY_DIRS})
target_compile_options(window PRIVATE ${GTKMM_CFLAGS_OTHER})
