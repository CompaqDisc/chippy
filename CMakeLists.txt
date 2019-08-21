cmake_minimum_required(VERSION 3.7.0)
project(Chippy VERSION 0.0.0.1 LANGUAGES CXX)

cmake_policy(SET CMP0072 NEW)
set(ExecutableName chippy)
set(SourceDir src)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include/)

file(GLOB_RECURSE SourceFiles
    ${SourceDir}/*.cc
)

##########

# Find thread package
set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

# Find GL and GLX package
find_package(OpenGL REQUIRED)

if (UNIX)
    find_package(X11 REQUIRED)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})

    # Set platform spesific output names
    set(Executable ${ExecutableName})
endif (UNIX)
if (WIN32)
    target_include_directories(${WinSDK})

    # Set platform spesific output names
    set(Executable ${ExecutableName}.exe)
endif (WIN32)

add_executable(${Executable} 
    ${SourceFiles}
    # Add more directories, files or CMake variables here to have them compile too!
)

target_link_libraries(${Executable}
    Threads::Threads
    OpenGL::OpenGL
    OpenGL::GL
    OpenGL::GLX
)

if (UNIX) 
    target_link_libraries(${Executable} 
        ${X11_LIBRARIES}
        PNG::PNG
    )
endif (UNIX)
if (WIN32)
    target_link_libraries(${Executable} 
        ${WinSDK}
    )
endif (WIN32)

#include(CTest)
#enable_testing()

#add_executable(Chippy main.cpp)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)