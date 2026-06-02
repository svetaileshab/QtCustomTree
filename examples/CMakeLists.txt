add_executable(demo
    main.cpp
    ../resources.qrc
)
target_link_libraries(demo QtCustomTree Qt5::Widgets)
