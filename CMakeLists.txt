cmake_minimum_required(VERSION 3.5)

# set the project name
project(Arrival_test)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Data generator
add_executable(data_gen src/generator/data_gen.cpp)
TARGET_INCLUDE_DIRECTORIES(data_gen PRIVATE src/ipclib)
TARGET_LINK_LIBRARIES(data_gen pthread ipclib)

# Data view client
add_executable(data_view_client src/view_client/data_view.cpp)
TARGET_INCLUDE_DIRECTORIES(data_view_client PRIVATE src/ipclib)
TARGET_LINK_LIBRARIES(data_view_client pthread ipclib)

# IPC library
add_library(ipclib SHARED src/ipclib/ipclib.cpp)
set_target_properties(ipclib PROPERTIES PUBLIC_HEADER src/ipclib/ipclib.h)
set_target_properties(ipclib PROPERTIES PUBLIC_HEADER src/ipclib/ipccomm.h)
