#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#


cmake_minimum_required(VERSION 2.8.4)

PROJECT(ObjectDetector)

#Need for json.hpp
#SET(CMAKE_CXX_FLAGS "-std=c++11")

include(dlib/cmake)

#include the subdirectory containing our libs
#add_subdirectory (cpp-json)
#include_directories(cpp-json)

# Tell CMake to compile a program.  We do this with the ADD_EXECUTABLE()
# statement which takes the name of the output executable and then a list of
# .cpp files to compile.  Here each example consists of only one .cpp file but
# in general you will make programs that const of many .cpp files.
#ADD_EXECUTABLE(assignment_learning_ex assignment_learning_ex.cpp)
# Then we tell it to link with dlib.
#TARGET_LINK_LIBRARIES(assignment_learning_ex dlib)


# Since there are a lot of examples I'm going to use a macro to simply this
# CMakeLists.txt file.  However, usually you will create only one executable in
# your cmake projects and use the syntax shown above.
MACRO(add_example name)
   ADD_EXECUTABLE(${name} ${name}.cpp json.hpp)
   TARGET_LINK_LIBRARIES(${name} dlib )
ENDMACRO()


#here we apply our macros 
add_example(main)
