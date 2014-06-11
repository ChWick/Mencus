project(MapUpgrader)

file(GLOB MapUpgrader_source_files *.cpp)
file(GLOB MapUpgrader_include_files *.hpp)

ADD_DEFINITIONS(-std=c++11)

set(MapUpgrader_include_directories
    ..
    .
    ${TINYXML2_INCLUDE_DIR}
)
set(MapUpgrader_link_libraries
    boost_system
    boost_thread
    boost_regex
    pthread
    ${TINYXML2_LIBRARIES}
)
set(EXECUTABLE_OUTPUT_PATH ../bin)
include_directories(${MapUpgrader_include_directories})
add_executable(MapUpgrader ${MapUpgrader_source_files} ${MapUpgrader_include_files})
target_link_libraries(MapUpgrader ${MapUpgrader_link_libraries})
