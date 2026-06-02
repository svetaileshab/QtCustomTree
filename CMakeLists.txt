cmake_minimum_required(VERSION 3.16)
project(QtCustomTree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Интерфейсы (header-only)
add_library(QtCustomTreeInterfaces INTERFACE)
target_include_directories(QtCustomTreeInterfaces INTERFACE interfaces)
target_link_libraries(QtCustomTreeInterfaces INTERFACE Qt6::Core)

# Основная библиотека
add_library(QtCustomTree STATIC
    src/QtTreeModel.cpp
    src/QtTreeSelection.cpp
    src/QtTreeDragHandler.cpp
    src/QtTreeStatusManager.cpp
    src/QtTreeContextMenu.cpp
    src/QtCustomTree.cpp
)
target_include_directories(QtCustomTree PUBLIC include)
target_link_libraries(QtCustomTree PUBLIC QtCustomTreeInterfaces Qt6::Widgets)

# Примеры
add_subdirectory(examples)
