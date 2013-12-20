project(Game)

file(GLOB Game_source_files *.cpp)
file(GLOB Game_include_files *.h)

ADD_DEFINITIONS(-std=c++11)

set(Game_include_directories
    ..
    .
    ${OIS_INCLUDE_DIR}
    ${OGRE_INCLUDE_DIR}
    ${CEGUI_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
)
set(Game_link_libraries
    boost_system
    boost_thread
    boost_regex
    pthread
    ${OIS_LIBRARIES}
    ${OGRE_LIBRARIES}
    ${CEGUI_LIBRARIES}
    ${TINYXML2_LIBRARIES}
)
set(EXECUTABLE_OUTPUT_PATH ../bin)
include_directories(${Game_include_directories})
add_executable(Game ${Game_source_files} ${Game_include_files})
target_link_libraries(Game ${Game_link_libraries})
