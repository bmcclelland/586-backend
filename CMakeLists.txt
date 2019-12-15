
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/CMakePCHCompiler)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CONAN_SYSTEM_INCLUDES ON)

project(mvc CXX)
#project(mvc CXX CXXPCH)

include(GNUInstallDirs)
include(FindPkgConfig)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

##-------------------------------    
set(MVC_SOURCES
    src/lib/app/app.cpp
    src/lib/auth/authenticator.cpp
    src/lib/auth/actor.cpp
    src/lib/auth/requirements.cpp
    src/lib/config/config.cpp
    src/lib/config/auth.cpp
    src/lib/config/http.cpp
    src/lib/config/sqlite.cpp
    src/lib/data/new.cpp
    src/lib/data/new-odb.cpp
    src/lib/idatabase/sqlitedatabase.cpp
    src/lib/iendpoint/getprojects.cpp
    src/lib/iendpoint/getproject.cpp
    src/lib/iendpoint/addproject.cpp
    src/lib/iendpoint/delproject.cpp
    src/lib/iendpoint/addtask.cpp
    src/lib/iendpoint/assigntask.cpp
    src/lib/iendpoint/unassigntask.cpp
    src/lib/iendpoint/gettask.cpp
    src/lib/iendpoint/addworker.cpp
    src/lib/iendpoint/getworkers.cpp
    src/lib/iendpoint/getworker.cpp
    src/lib/ihandler/httphandler.cpp
    src/lib/ihandler/getjwt.cpp
    src/lib/irouter/apirouter.cpp
    src/lib/iserver/httpserver.cpp
    src/lib/server/apiparse.cpp
    src/lib/server/apipath.cpp
    src/lib/util/color.cpp
    src/lib/http.cpp
    src/lib/httpclient.cpp
)

set(MVC_LIBS 
    debug asan
    odb-sqlite
    odb
    ${LIBS}
    ${CONAN_LIBS}
)

set(PROJROOT "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_command(
    OUTPUT ${PROJROOT}/src/lib/data/new-odb.cpp 
    DEPENDS ${PROJROOT}/src/lib/data/new.h
    COMMAND ${PROJROOT}/tools/odb
    "${PROJROOT}"
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(MVC_LIBS ${MVC_LIBS}
        stdc++fs
        #debug gcov
    )
endif ()

set(MVC_INCLUDES
    PRIVATE
        ${PROJROOT}/src/lib
    SYSTEM
        ${PROJROOT}/external/variant-1.1.6/include
        ${PROJROOT}/external/jwt-cpp-0.3.1/include
        ${PROJROOT}/external/boost-di/include
        ${PROJROOT}/external/nlohmann-json/include
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(MVC_COMPILE_OPTIONS PUBLIC
        -Wall 
        -Wextra 
        -Werror=return-type
        -Werror=switch
        -Wshadow
    )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(MVC_COMPILE_OPTIONS PUBLIC
            -Wall 
            -Wextra 
            -Werror=return-type
            -Werror=switch
            -Wfatal-errors
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wlogical-op
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wno-unknown-pragmas
            
            #-fprofile-arcs -ftest-coverage
            -fsanitize=address
            -g -ggdb
        )
    else()
        set(MVC_COMPILE_OPTIONS PUBLIC
            -Wall
            -Ofast 
            -flto
            -Wno-unknown-pragmas
            -Wl,-rpath,'$ORIGIN'
        )
    endif()

endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_library(lib ${MVC_SOURCES})
    target_include_directories(lib ${MVC_INCLUDES})
    #    target_precompiled_header(lib src/lib/precompile.h)
    target_link_libraries(lib ${MVC_LIBS})
    target_compile_options(lib ${MVC_COMPILE_OPTIONS})
    
    add_executable(mvc src/mvc/main.cpp)
    target_include_directories(mvc ${MVC_INCLUDES})
    target_link_libraries(mvc ${MVC_LIBS} lib)
    target_compile_options(mvc ${MVC_COMPILE_OPTIONS}
        -fsanitize=address
    )

    add_executable(test src/test/main.cpp)
    target_include_directories(test ${MVC_INCLUDES})
    target_link_libraries(test ${MVC_LIBS} lib)
    target_compile_options(test ${MVC_COMPILE_OPTIONS})
else()
    add_executable(mvc ${MVC_SOURCES} src/mvc/main.cpp)
    target_include_directories(mvc ${MVC_INCLUDES})
    #    target_precompiled_header(mvc src/lib/precompile.h)
    target_link_libraries(mvc ${MVC_LIBS})
    target_compile_options(mvc ${MVC_COMPILE_OPTIONS})
endif()
