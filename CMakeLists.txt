cmake_minimum_required(VERSION 3.0)

set(sources "CGNSCopier.cpp" "CGIOWrapper.cpp")

project(cgnsConverter)
add_executable(${PROJECT_NAME} "main.cpp" ${sources})
target_link_libraries(${PROJECT_NAME} "cgns" "hdf5" "dl")

