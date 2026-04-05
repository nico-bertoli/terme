cmake_minimum_required(VERSION 3.22)
project(terme VERSION 1.0.0)

#----------------------- base configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------- find dependencies
find_package(nbkit REQUIRED)
find_package(miniaudio REQUIRED CONFIG)

#----------------------- find source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/terme/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/terme/*.h"
)

if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in the terme directory!")
endif()

#----------------------- create library
add_library(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE nbkit::nbkit)
target_link_libraries(${PROJECT_NAME} PUBLIC miniaudio::miniaudio)

# On Windows, TUI apps often need the built-in Windows console headers
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 user32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_path(TERME_X11_INCLUDE_DIR NAMES X11/Xlib.h REQUIRED)
    find_library(TERME_X11_LIB NAMES X11 REQUIRED)
    target_include_directories(${PROJECT_NAME} PUBLIC ${TERME_X11_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${TERME_X11_LIB})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

#----------------------- installation logic
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY terme/
    DESTINATION include/terme
    FILES_MATCHING PATTERN "*.h"
)

#----------------------- Example executable
file(GLOB_RECURSE EXAMPLE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp") 

if(EXAMPLE_FILES)
    add_executable(terme_example ${EXAMPLE_FILES})
    target_link_libraries(terme_example PRIVATE ${PROJECT_NAME} nbkit::nbkit)
endif()

#----------------------- test
option(BUILD_TESTING "Build the testing tree" OFF)

if(BUILD_TESTING)
    enable_testing() 
    
    file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp") 
    
    if(TEST_FILES)
        add_executable(terme_test ${TEST_FILES})
        target_link_libraries(terme_test PRIVATE ${PROJECT_NAME} nbkit::nbkit)
        
        add_test(NAME terme_test COMMAND terme_test)
    endif()
endif()