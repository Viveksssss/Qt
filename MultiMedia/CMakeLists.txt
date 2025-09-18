cmake_minimum_required(VERSION 3.19)
project(MultiMedia LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(MultiMedia
    WIN32 MACOSX_BUNDLE
    main.cpp
    screencapturepreview.h screencapturepreview.cpp
    screenlistmodel.h screenlistmodel.cpp
    windowlistmodel.h windowlistmodel.cpp


)

target_link_libraries(MultiMedia
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::MultimediaWidgets
)
target_link_libraries(MultiMedia PRIVATE Qt6::Widgets)

include(GNUInstallDirs)

install(TARGETS MultiMedia
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET MultiMedia
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
