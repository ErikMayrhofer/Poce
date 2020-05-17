cmake_minimum_required(VERSION 3.16)

INCLUDE(FetchContent)

SET(box2d_PREFIX ${PROJECT_BINARY_DIR}/box2d)

# Needs fpic to link in shared lib on Linux
#IF(CMAKE_COMPILER_IS_GNUCXX)
#    SET(box2d_CXX_ARGS -DCMAKE_CXX_FLAGS=-fPIC)
#ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(box2d_CMAKE_ARGS
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        -DCMAKE_DEBUG_POSTFIX=_d
        -DBOX2D_INSTALL=ON
        -DBOX2D_BUILD_EXAMPLES=OFF
        ${box2d_CXX_ARGS}
        )

FetchContent_Declare(box2d-project
        PREFIX ${box2d_PREFIX}

        URL https://github.com/erincatto/Box2D/archive/v2.3.1.zip
        URL_MD5 f053d10662819c41f831357c4f3248f9

        # Box2D's source isn't in the top level directory so add a dummy file to set cmake right
        PATCH_COMMAND ${CMAKE_COMMAND} -E echo "ADD_SUBDIRECTORY(Box2D)" > <SOURCE_DIR>/CMakeLists.txt

        CMAKE_ARGS ${box2d_CMAKE_ARGS}
        )

#ExternalProject_Add(box2d-project
#        PREFIX ${box2d_PREFIX}
#
#        URL https://github.com/erincatto/Box2D/archive/v2.3.1.zip
#        URL_MD5 f053d10662819c41f831357c4f3248f9
#
#        # Box2D's source isn't in the top level directory so add a dummy file to set cmake right
#        PATCH_COMMAND ${CMAKE_COMMAND} -E echo ADD_SUBDIRECTORY(Box2D) > <SOURCE_DIR>/CMakeLists.txt
#
#        CMAKE_ARGS ${box2d_CMAKE_ARGS}
#        )
add_library(box2d INTERFACE)
add_dependencies(box2d box2d-project)
#
#ExternalProject_Get_property(box2d-project INSTALL_DIR)
#message("Box2DINSTALLDIR ${BOX2D_INCLUDE_DIR}")