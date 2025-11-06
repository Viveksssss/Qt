cmake_minimum_required(VERSION 3.19)
project(tmp LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(tmp
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    MessageType.h
    ChatMessage.h
    chatmessagemodel.h chatmessagemodel.cpp
    chatmessagedelegate.h chatmessagedelegate.cpp
    chatwidget.h chatwidget.cpp
)

target_link_libraries(tmp
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(tmp PRIVATE Qt6::Widgets)

include(GNUInstallDirs)

install(TARGETS tmp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET tmp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
