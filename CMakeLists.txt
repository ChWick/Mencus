cmake_minimum_required(VERSION 2.8)

project(Mencus)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#include(cotire)

find_package(Ogre)
find_package(OIS)
find_package(CEGUI)
find_package(tinyxml2)

add_subdirectory(Game)
add_subdirectory(FileConverter)
#add_subdirectory(MapEditor)

