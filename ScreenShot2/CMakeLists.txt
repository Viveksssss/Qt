cmake_minimum_required(VERSION 3.19)
project(ScreenShot2 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(ScreenShot2
    WIN32 MACOSX_BUNDLE
    main.cpp
    snipater.h snipater.cpp
    widget.h widget.cpp
    overlaywidget.h overlaywidget.cpp
    tools.h tools.cpp
    icons.qrc
)

target_link_libraries(ScreenShot2
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(ScreenShot2 PRIVATE Qt6::Core)
target_link_libraries(ScreenShot2 PRIVATE Qt6::Widgets)

include(GNUInstallDirs)

install(TARGETS ScreenShot2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ScreenShot2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
