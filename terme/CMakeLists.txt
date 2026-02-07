#--------------------------------------------- base setup
cmake_minimum_required(VERSION 3.22)
project(terme VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------------- create library
file(GLOB_RECURSE SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/terme/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/terme/*.h"
)

add_library(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

#--------------------------------------------- conan linking
find_package(nbkit REQUIRED)
find_package(SFML REQUIRED COMPONENTS system audio)

target_link_libraries(${PROJECT_NAME} PRIVATE nbkit::nbkit sfml::sfml)

# include X11 lib on linux
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11::X11)
endif()

#--------------------------------------------- test executable
file(
    GLOB TEST_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
)
    
add_executable(terme_test ${TEST_SRC})
target_link_libraries(terme_test PRIVATE ${PROJECT_NAME})
target_include_directories(terme_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test")

target_link_libraries(terme_test PRIVATE nbkit::nbkit sfml::sfml)
if(UNIX AND NOT APPLE)
    target_link_libraries(terme_test PRIVATE X11::X11)
endif()

# Copy resources to build directory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    add_custom_command(TARGET terme_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:terme_test>/resources"
    )
endif()
#--------------------------------------------- install rules
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY terme/
    DESTINATION include/terme
    FILES_MATCHING PATTERN "*.h"
)