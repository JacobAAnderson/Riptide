set(PROJECTNAME OSU_Riptide)

project(${PROJECTNAME})

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

add_subdirectory(Sensors)
add_subdirectory(DataLogger)
add_subdirectory(Acoms1)
add_subdirectory(Acoms2)
add_subdirectory(SerialComms)
