cmake_minimum_required(VERSION 3.16)
project(PoceExp)


set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

include(ExternalProject)

cmake_policy(SET CMP0075 OLD)

find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Boost COMPONENTS signals REQUIRED)


# ======= DLIB ============
set(DLIB_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/dlib)
# Needs fpic to link in shared lib on Linux
ExternalProject_Add(dlib-project
        #        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        #        GIT_TAG v2.3.1
        URL http://dlib.net/files/dlib-19.19.tar.bz2
        URL_MD5 34d70b26afe714432e8801374b49bb0a
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DLIB_INSTALL_LOCATION}
        )
set(DLIB_INCLUDE_DIR ${DLIB_INSTALL_LOCATION}/include)
link_directories(${DLIB_INSTALL_LOCATION}/lib)

# ======= BOX2d ===========
set(BOX2D_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/box2d)
# Needs fpic to link in shared lib on Linux
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(box2d_CXX_ARGS -DCMAKE_CXX_FLAGS=-fPIC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
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
ExternalProject_Add(box2d-project
#        GIT_REPOSITORY https://github.com/erincatto/box2d.git
#        GIT_TAG v2.3.1
        URL https://github.com/erincatto/Box2D/archive/v2.3.1.zip
        URL_MD5 f053d10662819c41f831357c4f3248f9
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${BOX2D_INSTALL_LOCATION}
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/cmake/box2dpatch.cmake <SOURCE_DIR>/CMakeLists.txt
        CMAKE_ARGS ${box2d_CMAKE_ARGS}
        )
set(BOX2D_INCLUDE_DIR ${BOX2D_INSTALL_LOCATION}/include)
link_directories(${BOX2D_INSTALL_LOCATION}/lib)

#=====================

include_directories(${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${BOX2D_INCLUDE_DIR} ${DLIB_INCLUDE_DIR} glad/include)
add_executable(PoceExp glad/src/glad.c
        Shader.cpp Shader.h ShaderProgram.cpp ShaderProgram.h
        VideoSource.cpp VideoSource.h Buffer.h main.cpp gltypemap.h gltypemap.cpp
        Texture.h VideoTexture.cpp VideoTexture.h Texture.cpp
        FaceDetector.cpp FaceDetector.h monitoring.h Window.cpp Window.h App.cpp App.h
        Poce/PoceGame.cpp Poce/PoceGame.h glwrap.h)
add_dependencies(PoceExp box2d-project)
add_dependencies(PoceExp dlib-project)

target_link_libraries(PoceExp ${OpenCV_LIBS} OpenGL::GL OpenGL::GLU OpenGL::GLX OpenGL::OpenGL ${CMAKE_DL_LIBS} box2d glfw dlib ${Boost_LIBRARIES})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()

file(COPY shaders DESTINATION .)
file(COPY shape_predictor_68_face_landmarks.dat DESTINATION .)
configure_file(fonts/arial.ttf fonts/arial.ttf COPYONLY)

add_executable(FontExample glad/src/glad.c fontexample.cpp Window.cpp)

target_link_libraries(FontExample
        OpenGL::GL OpenGL::GLU OpenGL::GLX
        OpenGL::OpenGL ${CMAKE_DL_LIBS} glfw dlib::dlib
        Box2D ${FREETYPE_LIBRARIES} ${FTGL_LIBRARIES} ${GLUT_LIBRARIES})