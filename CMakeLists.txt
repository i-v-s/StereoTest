cmake_minimum_required(VERSION 2.8)
project(stereo_test)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_ml
    opencv_video
    opencv_videoio
    opencv_features2d
    opencv_calib3d
    opencv_objdetect
    opencv_contrib
    opencv_legacy
    opencv_flann
)

add_executable(stereocalibrate stereocalibrate.cpp)
target_link_libraries(stereocalibrate
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_ml
    opencv_video
    opencv_videoio
    opencv_features2d
    opencv_calib3d
    opencv_objdetect
    opencv_contrib
    opencv_legacy
    opencv_flann
)
