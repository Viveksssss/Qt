cmake_minimum_required(VERSION 3.19)
project(CppChat LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)


qt_standard_project_setup()

# 这里资源文件qrc需要手动的显示添加:qt_add_resources,否则默认找不到
qt_add_resources(QT_RESOURCES "res.qrc")

qt_add_executable(CppChat
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${QT_RESOURCES}
    mainwindow.h mainwindow.cpp
    stylemanager.h stylemanager.cpp
    LoginInterface/forgotscreen.cpp LoginInterface/forgotscreen.h LoginInterface/loginscreen.cpp LoginInterface/loginscreen.h LoginInterface/registerscreen.cpp LoginInterface/registerscreen.h
    Properties/singleton.h
    Properties/global.h
    Properties/global.cpp
    httpmanager.h httpmanager.cpp
    tcpmanager.h tcpmanager.cpp
    config.ini
    usermanager.h usermanager.cpp
    MainInterface/mainscreen.h MainInterface/mainscreen.cpp
    MainInterface/toptitlepart.h MainInterface/toptitlepart.cpp
    MainInterface/sidebarpart.h MainInterface/sidebarpart.cpp
    MainInterface/FriendsList//friendslistpart.h MainInterface//FriendsList/friendslistpart.cpp
    MainInterface/FriendsList/frienditem.h
    MainInterface/FriendsList/friendsmodel.h MainInterface/FriendsList/friendsmodel.cpp
    MainInterface/FriendsList/frienditemdelegate.h MainInterface/FriendsList/frienditemdelegate.cpp
    MainInterface/Chat/chatpart.h MainInterface/Chat/chatpart.cpp
    MainInterface/Chat/chattoparea.h MainInterface/Chat/chattoparea.cpp
    MainInterface/Chat/chatarea.h MainInterface/Chat/chatarea.cpp
)



configure_file(${CMAKE_SOURCE_DIR}/config.ini
               ${CMAKE_BINARY_DIR}/config.ini COPYONLY)

target_link_libraries(CppChat
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Network
)
target_link_libraries(CppChat PRIVATE Qt6::Core)
target_link_libraries(CppChat PRIVATE Qt6::Core)
target_link_libraries(CppChat PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Core Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Core)
target_link_libraries(CppChat PRIVATE Qt6::Core)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)
target_link_libraries(CppChat PRIVATE Qt6::Widgets)


target_include_directories(CppChat PRIVATE ${ASIO_INCLUDE_DIR})


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
