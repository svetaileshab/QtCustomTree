cmake_minimum_required(VERSION 3.16)
project(QtCustomTree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(QtCustomTreeInterfaces INTERFACE)
target_include_directories(QtCustomTreeInterfaces INTERFACE interfaces)
target_link_libraries(QtCustomTreeInterfaces INTERFACE Qt5::Core)

add_library(QtCustomTree STATIC
    include/QtCustomTree/QtTreeModel.h
    include/QtCustomTree/QtTreeSelection.h
    include/QtCustomTree/QtTreeDragHandler.h
    include/QtCustomTree/QtTreeStatusManager.h
    include/QtCustomTree/QtTreeContextMenu.h
    include/QtCustomTree/QtCustomTree.h
    src/QtTreeModel.cpp
    src/QtTreeSelection.cpp
    src/QtTreeDragHandler.cpp
    src/QtTreeStatusManager.cpp
    src/QtTreeContextMenu.cpp
    src/QtCustomTree.cpp
)
target_include_directories(QtCustomTree PUBLIC include)
target_link_libraries(QtCustomTree PUBLIC QtCustomTreeInterfaces Qt5::Widgets)

add_subdirectory(examples)
