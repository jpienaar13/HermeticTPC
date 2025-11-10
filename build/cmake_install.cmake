# Install script for directory: /home/hargy/Science/GEANT4/HermeticTPC

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "..")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/../bin" TYPE EXECUTABLE FILES "/home/hargy/Science/GEANT4/HermeticTPC/build/bin/hermeticTPC")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC"
         OLD_RPATH "/home/hargy/Science/ROOT/root-install/lib:/home/hargy/Science/G4V2/geant4-v10.7.4-install/lib64:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/../bin/hermeticTPC")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/hargy/Science/GEANT4/HermeticTPC/build/CMakeFiles/hermeticTPC.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/hargy/Science/GEANT4/HermeticTPC/build/libHTPC.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/../include/HTPC" TYPE FILE FILES
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCAnalysisManager.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCDetectorConstruction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCDetectorHit.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCEventAction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCEventData.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCParticleSource.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCParticleSourceMessenger.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCPhysicsList.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCPrimaryGeneratorAction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCRunAction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCSensitiveDetector.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCStackingAction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCStackingActionMessenger.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/HTPCSteppingAction.hh"
    "/home/hargy/Science/GEANT4/HermeticTPC/include/fileMerger.hh"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/hargy/Science/GEANT4/HermeticTPC/build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/hargy/Science/GEANT4/HermeticTPC/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
