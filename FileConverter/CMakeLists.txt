project(FileConverter)

file(GLOB FileConverter_source_files *.cpp)
file(GLOB FileConverter_include_files *.hpp)

ADD_DEFINITIONS(-std=c++11)

set(FileConverter_include_directories
    ..
    .
    ${BOOST_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
)
set(FileConverter_link_libraries
	${BOOST_LIBRARIES}
    pthread
    ${TINYXML2_LIBRARIES}
)
set(EXECUTABLE_OUTPUT_PATH ../bin)
include_directories(${FileConverter_include_directories})

if (APPLE)
	include_directories(/opt/local/include)
endif()

add_executable(FileConverter ${FileConverter_source_files} ${FileConverter_include_files})
target_link_libraries(FileConverter ${FileConverter_link_libraries})