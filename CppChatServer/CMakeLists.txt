cmake_minimum_required(VERSION 3.10.0)
project(GateWayServer VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)

pkg_check_modules(JSON REQUIRED REQUIRED IMPORTED_TARGET nlohmann_json)


add_executable(GateWayServer 
    main.cpp
    GateWayServer.cpp
    Session.cpp
    LogicSystem.cpp
    
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::JSON)

