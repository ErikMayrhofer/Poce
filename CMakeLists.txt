cmake_minimum_required(VERSION 3.12)
project(PoceExp)

set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

add_subdirectory(dlib dlib_build)



find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Freetype REQUIRED)
find_package(Box2D REQUIRED)
find_package(FTGL REQUIRED)
find_package(GLUT REQUIRED)

message(STATUS ${FREETYPE_INCLUDE_DIRS})

include_directories(${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS} ${FTGL_INCLUDE_DIR} glad/include)
add_executable(PoceExp glad/src/glad.c
        Shader.cpp Shader.h ShaderProgram.cpp ShaderProgram.h
        VideoSource.cpp VideoSource.h Buffer.h main.cpp gltypemap.h gltypemap.cpp
        Texture.h VideoTexture.cpp VideoTexture.h Texture.cpp
        FaceDetector.cpp FaceDetector.h monitoring.h Window.cpp Window.h App.cpp App.h
        Poce/PoceGame.cpp Poce/PoceGame.h Poce/TestTest.cpp glwrap.h profiler.cpp)

target_link_libraries(PoceExp ${OpenCV_LIBS}
        OpenGL::GL OpenGL::GLU OpenGL::GLX
        OpenGL::OpenGL ${CMAKE_DL_LIBS} glfw dlib::dlib
        Box2D ${FREETYPE_LIBRARIES} ${FTGL_LIBRARIES}
        )

file(COPY shaders DESTINATION .)
file(COPY shape_predictor_68_face_landmarks.dat DESTINATION .)
configure_file(fonts/arial.ttf fonts/arial.ttf COPYONLY)

add_executable(FontExample glad/src/glad.c fontexample.cpp Window.cpp)

target_link_libraries(FontExample
        OpenGL::GL OpenGL::GLU OpenGL::GLX
        OpenGL::OpenGL ${CMAKE_DL_LIBS} glfw dlib::dlib
        Box2D ${FREETYPE_LIBRARIES} ${FTGL_LIBRARIES} ${GLUT_LIBRARIES})