cmake_minimum_required(VERSION 3.19)
project(PDFViewer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets PdfWidgets )
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(PDFViewer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.h mainwindow.cpp
    images.qrc
)

target_link_libraries(PDFViewer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::PdfWidgets
)
target_link_libraries(PDFViewer PRIVATE Qt6::Widgets Qt6::PdfWidgets)

include(GNUInstallDirs)

install(TARGETS PDFViewer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET PDFViewer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
