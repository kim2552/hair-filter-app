# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SRC_DIR src/main/cpp)

include_directories(${OpenCV_DIR}/jni/include)
add_library( loc_core SHARED IMPORTED )
set_target_properties(loc_core PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_core.so)
add_library( loc_face SHARED IMPORTED )
set_target_properties(loc_face PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_face.so)
add_library( loc_highgui SHARED IMPORTED )
set_target_properties(loc_highgui PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_highgui.so)
add_library( loc_objdetect SHARED IMPORTED )
set_target_properties(loc_objdetect PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_objdetect.so)
add_library( loc_imgproc SHARED IMPORTED )
set_target_properties(loc_imgproc PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_imgproc.so)
add_library( loc_videoio SHARED IMPORTED )
set_target_properties(loc_videoio PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_videoio.so)
add_library( loc_video SHARED IMPORTED )
set_target_properties(loc_video PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_video.so)
add_library( loc_calib3d SHARED IMPORTED )
set_target_properties(loc_calib3d PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_calib3d.so)
add_library( loc_imgcodecs SHARED IMPORTED )
set_target_properties(loc_imgcodecs PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_imgcodecs.so)
add_library( loc_dnn SHARED IMPORTED )
set_target_properties(loc_dnn PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_dnn.so)
add_library( loc_features2d SHARED IMPORTED )
set_target_properties(loc_features2d PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_features2d.so)
add_library( loc_flann SHARED IMPORTED )
set_target_properties(loc_flann PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_flann.so)
add_library( loc_fuzzy SHARED IMPORTED )
set_target_properties(loc_fuzzy PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_fuzzy.so)
add_library( loc_gapi SHARED IMPORTED )
set_target_properties(loc_gapi PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_gapi.so)

add_library(3rdparty INTERFACE)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/cereal)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/Eigen)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/eigen3-nnls)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/glm)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/mexplus)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/nanoflann)
target_include_directories(3rdparty INTERFACE ${SRC_DIR}/3rdparty/toml11)

add_library(eos INTERFACE)
target_include_directories(eos INTERFACE ${SRC_DIR})
target_include_directories(eos INTERFACE ${SRC_DIR}/eos)

target_link_libraries(
        eos
        INTERFACE
        3rdparty
)

add_library(include INTERFACE)
target_include_directories(include INTERFACE ${SRC_DIR}/include)

add_library(config INTERFACE)
target_include_directories(include INTERFACE ${SRC_DIR}/config)

add_library(objects INTERFACE)
target_include_directories(include INTERFACE ${SRC_DIR}/objects)

add_library(render INTERFACE)
target_include_directories(render INTERFACE ${SRC_DIR}/render)

add_library(face INTERFACE)
target_include_directories(face INTERFACE ${SRC_DIR}/face)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        media-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SRC_DIR}/glad.c
        ${SRC_DIR}/render/stb.cpp
        ${SRC_DIR}/include/tinyobj/tiny_obj_loader.cc

        ${SRC_DIR}/VideoRenderer.cpp
        ${SRC_DIR}/VideoRendererContext.cpp
        ${SRC_DIR}/VideoRendererJNI.cpp
        ${SRC_DIR}/CommonUtils.cpp
        ${SRC_DIR}/GLUtils.cpp
        ${SRC_DIR}/GLVideoRendererYUV420.cpp
        ${SRC_DIR}/GLVideoRendererYUV420Filter.cpp

        ${SRC_DIR}/config/AppConfig.cpp

        ${SRC_DIR}/render/EBO.cpp
        ${SRC_DIR}/render/VBO.cpp
        ${SRC_DIR}/render/VAO.cpp
        ${SRC_DIR}/render/ShaderClass.cpp
        ${SRC_DIR}/render/Texture.cpp
        ${SRC_DIR}/render/Camera.cpp
        ${SRC_DIR}/render/Mesh.cpp
        ${SRC_DIR}/render/Model.cpp
        ${SRC_DIR}/render/ModelObj.cpp

        ${SRC_DIR}/objects/CameraImage.cpp
        ${SRC_DIR}/objects/FaceMask.cpp
        ${SRC_DIR}/objects/HairObject.cpp

        ${SRC_DIR}/face/FaceDetect.cpp
        ${SRC_DIR}/face/FaceMesh.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

find_library( # Sets the name of the path variable.
        GLESv3-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        GLESv3 )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        media-lib
        loc_core
        loc_face
        loc_highgui
        loc_objdetect
        loc_imgproc
        loc_videoio
        loc_video
        loc_calib3d
        loc_imgcodecs
        loc_dnn
        loc_features2d
        loc_flann
        loc_fuzzy
        loc_gapi
        android
        ${log-lib}
        ${GLESv3-lib}
        include
        config
        objects
        render
        face
        eos
        3rdparty)

########     CDep package for shaderc    ######################
# pre-requirement:
#    in terminal, cd {project_dir}; ./cdep
# that will generate .cdep/modules/cdep-dependencies-config.cmake
# the next 2 lines will be able to pull it into the project
# shaderc binaries are saved on github:
#      https://github.com/ggfan/shaderc/releases
# CDep fetches the lib with config file in ${project_dir}/cdep.yml
# ******* Most Importantly, CDep full documentation is @:
#  https://github.com/google/cdep
get_filename_component(cdep-dependencies_DIR
        ${SRC_DIR}/../../../../.cdep/modules  ABSOLUTE)
find_package(cdep-dependencies REQUIRED)
add_all_cdep_dependencies(media-lib)
