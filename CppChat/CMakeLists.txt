cmake_minimum_required(VERSION 3.19)
project(CppChat LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()


# 这里资源文件qrc需要手动的显示添加:qt_add_resources,否则默认找不到
qt_add_resources(QT_RESOURCES "res.qrc")

qt_add_executable(CppChat
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${QT_RESOURCES}
    mainwindow.h mainwindow.cpp
    loginscreen.h loginscreen.cpp
    registerscreen.h registerscreen.cpp
    stylemanager.h stylemanager.cpp

)

target_link_libraries(CppChat
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS CppChat
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CppChat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
