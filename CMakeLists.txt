cmake_minimum_required(VERSION 3.14)

if (WIN32)
    project(Network_Chess_Client LANGUAGES CXX)
    project(Network_Chess_Server LANGUAGES CXX)
elseif(UNIX)
    project(Network_Chess_Client)
    project(Network_Chess_Server)
endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

#=================== INCLUSION OF Project Files ====================#
set(CLIENT_DIR "${CMAKE_SOURCE_DIR}/Client")
set(COMMON_DIR "${CMAKE_SOURCE_DIR}/Common")
set(SERVER_DIR "${CMAKE_SOURCE_DIR}/Server")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/Resources")

include_directories(${CLIENT_DIR})
include_directories(${COMMON_DIR})
include_directories(${SERVER_DIR})
include_directories(${RESOURCES_DIR})

file(GLOB SOURCES
    Client/ChessView.cpp
    Client/ChessViewModel.cpp
    Client/SwitchPawnDialog.cpp
    Server/ChessModel.cpp
    Server/ChessServer.cpp
    Resources/resources.qrc
    )

set(ignoreMe "${QT_QMAKE_EXECUTABLE}")

#=================== SETUP EXECTUABLE ====================#
# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CLIENT_DIR})

# Add the executable
if (WIN32)
    add_executable(Network_Chess_Client WIN32 ${CLIENT_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Server WIN32 ${SERVER_DIR}/main.cpp ${SOURCES})
elseif(UNIX)
    add_executable(Network_Chess_Client ${CLIENT_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Server ${SERVER_DIR}/main.cpp ${SOURCES})
endif()

# Add the target includes for Network_Chess
target_include_directories(Network_Chess_Client PUBLIC ${CLIENT_DIR})
target_include_directories(Network_Chess_Server PUBLIC ${SERVER_DIR})
target_include_directories(Network_Chess_Client PUBLIC ${COMMON_DIR})
target_include_directories(Network_Chess_Server PUBLIC ${COMMON_DIR})
target_include_directories(Network_Chess_Client PUBLIC ${RESOURCES_DIR})

#===================== LINKING LIBRARIES =======================#
target_link_libraries(Network_Chess_Client Qt5::Widgets Qt5::Network)
target_link_libraries(Network_Chess_Server Qt5::Widgets Qt5::Network)
