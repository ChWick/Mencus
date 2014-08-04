cmake_minimum_required(VERSION 2.8)

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_BIT)")
endif()

project(Mencus)

# set version
set (MENCUS_VERSION_CODE 12)
set (MENCUS_VERSION_NAME 0.0.12)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set (MENCUS_TEMPLATES_DIR "${CMAKE_MODULE_PATH}/templates")
#include(cotire)

set (MENCUS_STATIC_DEPENDENCIES FALSE)
if (ANDROID)
  message(STATUS "Using static dependencies. Android needs them.")
  set (MENCUS_STATIC_DEPENDENCIES TRUE)
endif()


include(toolchain/AddSdkPaths)

if (APPLE)
	# On apple we use the framework for ogre, ois is included
else()
	# On other systems find Ogre and OIS
	find_package(Ogre)
	find_package(OIS)
endif()

# On all systems
find_package(CEGUI)
find_package(tinyxml2)
find_package(boost)

if(ANDROID)
  include(toolchain/AndroidGame)
else()
  add_subdirectory(Game)
  add_subdirectory(FileConverter)
  add_subdirectory(MapUpgrader)
endif()

