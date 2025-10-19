cmake_minimum_required(VERSION 3.10.0)
project(GateWayServer VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
# Set runtime output directory globally to avoid referencing the target before it's created

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

cmake_policy(SET CMP0167 NEW)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS filesystem system)


pkg_check_modules(REDIS REQUIRED REQUIRED IMPORTED_TARGET hiredis)
pkg_check_modules(JSON REQUIRED REQUIRED IMPORTED_TARGET nlohmann_json)
pkg_check_modules(GRPC REQUIRED REQUIRED IMPORTED_TARGET grpc)
pkg_check_modules(GRPCPP REQUIRED REQUIRED IMPORTED_TARGET grpc++)
pkg_check_modules(PROTOBUF REQUIRED IMPORTED_TARGET protobuf)
pkg_check_modules(ABSL REQUIRED IMPORTED_TARGET absl_strings)


add_executable(GateWayServer 
    main.cpp
    GateWayServer.cpp
    Session.cpp
    LogicSystem.cpp
    message.grpc.pb.cc
    message.pb.cc
    VerifyClient.h
    ConfigManager.h
    AsioPool.cpp
    RedisManager.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::JSON)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GRPC)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GRPCPP)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::PROTOBUF)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::ABSL)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::REDIS)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::filesystem Boost::system)



    