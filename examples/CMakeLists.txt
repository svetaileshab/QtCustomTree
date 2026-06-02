add_executable(demo
    main.cpp
    FakeDataSource.h
    FakeFileDataSource.h
    FakeOrgDataSource.h
    FakeGeoDataSource.h
    ../resources.qrc
)
target_link_libraries(demo QtCustomTree Qt5::Widgets)
