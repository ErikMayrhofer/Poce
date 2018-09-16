cmake_minimum_required(VERSION 3.10)
project(PoceExp)

set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE GLVND)

add_subdirectory(dlib dlib_build)

find_package(OpenCV REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
add_executable(PoceExp
        Shader.cpp Shader.h ShaderProgram.cpp ShaderProgram.h
        VideoSource.cpp VideoSource.h Buffer.h main.cpp gltypemap.h gltypemap.cpp
        Texture.h VideoTexture.cpp VideoTexture.h Texture.cpp
        FaceDetector.cpp FaceDetector.h monitoring.h Window.cpp Window.h App.cpp App.h
        Poce/PoceGame.cpp Poce/PoceGame.h)

target_link_libraries(PoceExp ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${BOOST_LIBRARIES} GLEW::GLEW glfw dlib::dlib)

#file(GLOB shaders shaders/**)
file(COPY shaders DESTINATION .)
file(COPY shape_predictor_68_face_landmarks.dat DESTINATION .)

project(DlibTest)
add_executable(DlibTest FaceDetector.h FaceDetector.cpp main_dlib.cpp Poce/PoceGame.cpp Poce/PoceGame.h)
target_link_libraries(DlibTest ${OpenCV_LIBS} dlib::dlib)
file(COPY shape_predictor_68_face_landmarks.dat DESTINATION .)