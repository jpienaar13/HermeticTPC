cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(hermeticTPC LANGUAGES C CXX)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB gen_sources ${PROJECT_SOURCE_DIR}/src/generators/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB gen_headers ${PROJECT_SOURCE_DIR}/include/generators/*.hh)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "lib")

add_executable(hermeticTPC hermeticTPC.cc)
add_library(HTPC STATIC ${sources} ${headers} ${gen_sources} ${gen_headers})

target_link_libraries(hermeticTPC PRIVATE HTPC)
# Place of working directory
option(MAKE_STYLE "Install to $G4WORKDIR like old Makefile style" OFF)
if(MAKE_STYLE)
  set(CMAKE_INSTALL_PREFIX $ENV{G4WORKDIR})
  set(WORK_DIR_NAME bin/Linux-g++)
else()
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ".." CACHE PATH "..." FORCE)
  endif()
  message("\t CMAKE INSTALL PREFIX : "  ${CMAKE_INSTALL_PREFIX})
  # set(CMAKE_INSTALL_PREFIX ..)
  # set(WORK_DIR_NAME bin)
endif()

# Setting ROOT
find_package(ROOT CONFIG REQUIRED)
target_link_libraries(hermeticTPC PRIVATE ROOT::MathCore ROOT::Hist ROOT::Tree)
target_link_libraries(HTPC PRIVATE ROOT::MathCore ROOT::Hist ROOT::Tree)

# Setting Geant4
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
target_link_libraries(hermeticTPC PRIVATE ${Geant4_LIBRARIES})
target_link_libraries(HTPC PRIVATE ${Geant4_LIBRARIES})

# Source directory
target_include_directories(hermeticTPC PRIVATE ${PROJECT_SOURCE_DIR}/include  ${PROJECT_SOURCE_DIR}/include/generators)
target_include_directories(HTPC PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/generators)
target_link_libraries(hermeticTPC PRIVATE HTPC)

# Compiler options
target_compile_features(hermeticTPC PRIVATE cxx_std_11)
target_compile_features(HTPC PRIVATE cxx_std_11)

# Install binaries
if(MAKE_STYLE)
    install(TARGETS hermeticTPC DESTINATION ${WORK_DIR_NAME})
    install(TARGETS HTPC DESTINATION ${WORK_DIR_NAME})
else()
    install(TARGETS hermeticTPC DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS HTPC
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )
    install(FILES ${headers}
             DESTINATION ${CMAKE_INSTALL_PREFIX}/include/HTPC
             COMPONENT Headers
    )
endif()
