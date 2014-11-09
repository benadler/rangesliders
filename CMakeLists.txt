cmake_minimum_required(VERSION 2.8.8)
project(rangesliders)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_definitions(-std=c++11 -fPIC)
include_directories(
    /usr/include
    # add the binary tree to the search path for include files so that we will find config.h
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SRC_FILES
mainwindow
widgetgradienteditor
rangeslider
)

set(UI_FILES mainwindow.ui)
qt5_wrap_ui(UI_SRCS ${UI_FILES})

add_executable(rangesliders ${SRC_FILES} ${UI_SRCS} ${RESOURCE_SRCS})
qt5_use_modules(rangesliders Core Gui Widgets)
set_target_properties(rangesliders PROPERTIES AUTOMOC TRUE)
